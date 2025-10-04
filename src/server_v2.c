/*
 * Film Negative API Server v2.0
 * Production-ready HTTP server for Railway deployment
 * Enhanced with proper multipart parsing, security, and error handling
 */

#include "film_processor.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define DEFAULT_PORT 8080
#define MAX_BUFFER 20971520  // 20MB max request size
#define MAX_CLIENTS 200
#define TEMP_DIR "/tmp"

// Platform compatibility
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

// Global server state
int server_running = 1;

// Configuration
typedef struct {
    int port;
    int max_connections;
    int request_timeout;
} Config;

Config config = {
    .port = DEFAULT_PORT,
    .max_connections = MAX_CLIENTS,
    .request_timeout = 30
};

// Enhanced logging with levels
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

void log_msg(LogLevel level, const char *message) {
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("[%s] [%s] %s\n", timestamp, level_str[level], message);
    fflush(stdout);
}

// Send HTTP response with proper headers
void send_response(int client_socket, int status_code, const char *status_text,
                   const char *content_type, const unsigned char *body, size_t body_len) {
    char header[2048];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "X-Content-Type-Options: nosniff\r\n"
        "X-Frame-Options: DENY\r\n"
        "X-XSS-Protection: 1; mode=block\r\n"
        "Server: FilmProcessor/2.0\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_text, content_type, body_len);

    // Send header
    ssize_t sent = send(client_socket, header, header_len, MSG_NOSIGNAL);
    if (sent < 0) {
        log_msg(LOG_ERROR, "Failed to send response header");
        return;
    }

    // Send body in chunks to handle large files
    if (body && body_len > 0) {
        size_t total_sent = 0;
        while (total_sent < body_len) {
            ssize_t chunk_sent = send(client_socket, body + total_sent,
                                      body_len - total_sent, MSG_NOSIGNAL);
            if (chunk_sent < 0) {
                log_msg(LOG_ERROR, "Failed to send response body");
                return;
            }
            total_sent += chunk_sent;
        }
    }
}

// Send JSON error response
void send_error(int client_socket, int status_code, const char *message) {
    char json[1024];
    snprintf(json, sizeof(json),
        "{\"error\":\"%s\",\"status\":%d,\"timestamp\":%ld}",
        message, status_code, (long)time(NULL));
    send_response(client_socket, status_code, "Error", "application/json",
                  (unsigned char *)json, strlen(json));

    char log_buf[1024];
    snprintf(log_buf, sizeof(log_buf), "Error %d: %s", status_code, message);
    log_msg(LOG_ERROR, log_buf);
}

// Extract boundary from Content-Type header
char* extract_boundary(const char *content_type) {
    const char *boundary_marker = "boundary=";
    char *boundary_start = strstr(content_type, boundary_marker);
    if (!boundary_start) return NULL;

    boundary_start += strlen(boundary_marker);

    // Remove quotes if present
    if (*boundary_start == '"') boundary_start++;

    char *boundary = malloc(256);
    if (!boundary) return NULL;

    int i = 0;
    while (boundary_start[i] && boundary_start[i] != '"' &&
           boundary_start[i] != '\r' && boundary_start[i] != '\n' && i < 255) {
        boundary[i] = boundary_start[i];
        i++;
    }
    boundary[i] = '\0';

    return boundary;
}

// Enhanced multipart parser - robust version
int parse_multipart_image(const char *body, size_t body_len, const char *boundary,
                          unsigned char **image_data, size_t *image_size) {
    if (!body || !boundary || body_len == 0) return 0;

    // Skip headers until we find \r\n\r\n (marks start of binary data)
    const char *data_start = strstr(body, "\r\n\r\n");
    if (!data_start) {
        log_msg(LOG_WARN, "Header separator not found");
        return 0;
    }
    data_start += 4;  // Skip \r\n\r\n

    // Find any boundary marker after the data (try multiple patterns)
    const char *data_end = NULL;

    // Pattern 1: \r\n--boundary--
    char pattern1[300];
    snprintf(pattern1, sizeof(pattern1), "\r\n--%s--", boundary);
    data_end = strstr(data_start, pattern1);

    if (!data_end) {
        // Pattern 2: \r\n--boundary\r\n
        char pattern2[300];
        snprintf(pattern2, sizeof(pattern2), "\r\n--%s\r\n", boundary);
        data_end = strstr(data_start, pattern2);
    }

    if (!data_end) {
        // Pattern 3: Just look for any --boundary
        char pattern3[300];
        snprintf(pattern3, sizeof(pattern3), "--%s", boundary);
        const char *temp = data_start;
        while ((temp = strstr(temp + 1, pattern3)) != NULL) {
            // Make sure it's preceded by \r\n
            if (temp > data_start + 2 && *(temp-2) == '\r' && *(temp-1) == '\n') {
                data_end = temp - 2;
                break;
            }
        }
    }

    if (!data_end) {
        log_msg(LOG_WARN, "Boundary end not found - using body end");
        // As last resort, use remaining body
        data_end = body + body_len;
        // Trim any trailing whitespace/newlines
        while (data_end > data_start && (*(data_end-1) == '\r' || *(data_end-1) == '\n' || *(data_end-1) == '-')) {
            data_end--;
        }
    }

    *image_size = data_end - data_start;

    if (*image_size == 0 || *image_size > MAX_BUFFER) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Invalid image size: %zu", *image_size);
        log_msg(LOG_WARN, msg);
        return 0;
    }

    *image_data = malloc(*image_size);
    if (!*image_data) {
        log_msg(LOG_ERROR, "Memory allocation failed for image data");
        return 0;
    }

    memcpy(*image_data, data_start, *image_size);

    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "Extracted image data: %zu bytes", *image_size);
    log_msg(LOG_DEBUG, log_buf);

    return 1;
}

// Handle POST request with improved parsing
void handle_post_request(int client_socket, const char *path, const char *headers,
                         const char *body, size_t body_len) {
    ProcessMode mode;

    if (strcmp(path, "/api/to-negative") == 0) {
        mode = MODE_TO_NEGATIVE;
        log_msg(LOG_INFO, "Processing: to-negative");
    } else if (strcmp(path, "/api/to-positive") == 0) {
        mode = MODE_TO_POSITIVE;
        log_msg(LOG_INFO, "Processing: to-positive");
    } else {
        send_error(client_socket, 404, "Endpoint not found");
        return;
    }

    // Validate request size
    if (body_len > MAX_BUFFER) {
        send_error(client_socket, 413, "Request too large");
        return;
    }

    // Extract Content-Type
    char content_type[512] = {0};
    const char *ct = strstr(headers, "Content-Type:");
    if (!ct) {
        send_error(client_socket, 400, "Content-Type header missing");
        return;
    }
    sscanf(ct, "Content-Type: %511[^\r\n]", content_type);

    // Verify multipart/form-data
    if (strstr(content_type, "multipart/form-data") == NULL) {
        send_error(client_socket, 400, "Content-Type must be multipart/form-data");
        return;
    }

    // Extract boundary
    char *boundary = extract_boundary(content_type);
    if (!boundary) {
        send_error(client_socket, 400, "Invalid multipart boundary");
        return;
    }

    // Parse multipart data
    unsigned char *image_data = NULL;
    size_t image_size = 0;

    if (!parse_multipart_image(body, body_len, boundary, &image_data, &image_size)) {
        free(boundary);
        send_error(client_socket, 400, "Failed to parse image from multipart data");
        return;
    }
    free(boundary);

    // Process image
    log_msg(LOG_INFO, "Processing image...");
    ImageResult result = process_image(image_data, image_size, mode);
    free(image_data);

    if (!result.success) {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "Image processing failed: %s", result.error_message);
        send_error(client_socket, 500, error_msg);
        return;
    }

    // Generate unique temporary filename
    char temp_filename[512];
    snprintf(temp_filename, sizeof(temp_filename), "%s/film_%ld_%d.jpg",
             TEMP_DIR, (long)time(NULL), rand() % 10000);

    // Write to file
    int write_success = stbi_write_jpg(temp_filename, result.width, result.height,
                                        result.channels, result.data, 90);
    free_image_result(&result);

    if (!write_success) {
        send_error(client_socket, 500, "Failed to encode output image");
        return;
    }

    // Ensure file is fully written to disk
    sync();

    // Read file
    FILE *fp = fopen(temp_filename, "rb");
    if (!fp) {
        unlink(temp_filename);
        send_error(client_socket, 500, "Failed to read output image");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= 0) {
        fclose(fp);
        unlink(temp_filename);
        send_error(client_socket, 500, "Invalid output file size");
        return;
    }

    unsigned char *file_data = malloc(file_size);
    if (!file_data) {
        fclose(fp);
        unlink(temp_filename);
        send_error(client_socket, 500, "Memory allocation failed");
        return;
    }

    size_t read_size = fread(file_data, 1, file_size, fp);
    fclose(fp);
    unlink(temp_filename);

    if (read_size != (size_t)file_size) {
        char log_buf[256];
        snprintf(log_buf, sizeof(log_buf), "File size mismatch: expected %ld, read %zu", file_size, read_size);
        log_msg(LOG_ERROR, log_buf);
        free(file_data);
        send_error(client_socket, 500, "Failed to read complete file");
        return;
    }

    char log_buf[256];
    snprintf(log_buf, sizeof(log_buf), "Sending JPEG response: %zu bytes", read_size);
    log_msg(LOG_DEBUG, log_buf);

    log_msg(LOG_INFO, "Image processed successfully");
    send_response(client_socket, 200, "OK", "image/jpeg", file_data, file_size);
    free(file_data);
}

// Handle GET request
void handle_get_request(int client_socket, const char *path) {
    if (strcmp(path, "/health") == 0 || strcmp(path, "/health/") == 0) {
        const char *response = "{\"status\":\"healthy\",\"service\":\"film-processor\",\"version\":\"2.0\"}";
        send_response(client_socket, 200, "OK", "application/json",
                      (unsigned char *)response, strlen(response));
        log_msg(LOG_DEBUG, "Health check OK");
    } else if (strcmp(path, "/") == 0) {
        const char *response =
            "{\"service\":\"Film Negative Processor\","
            "\"version\":\"2.0.0\","
            "\"endpoints\":[\"/api/to-negative\",\"/api/to-positive\",\"/health\"],"
            "\"documentation\":\"https://github.com/yourusername/film-processor\"}";
        send_response(client_socket, 200, "OK", "application/json",
                      (unsigned char *)response, strlen(response));
    } else {
        send_error(client_socket, 404, "Not found");
    }
}

// Handle OPTIONS (CORS preflight)
void handle_options_request(int client_socket) {
    send_response(client_socket, 204, "No Content", "text/plain", NULL, 0);
}

// Client handler thread
void* handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char *buffer = malloc(MAX_BUFFER);
    if (!buffer) {
        close(client_socket);
        log_msg(LOG_ERROR, "Failed to allocate buffer for client");
        return NULL;
    }

    // Receive request with timeout
    struct timeval timeout;
    timeout.tv_sec = config.request_timeout;
    timeout.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    ssize_t received = recv(client_socket, buffer, MAX_BUFFER - 1, 0);
    if (received <= 0) {
        free(buffer);
        close(client_socket);
        return NULL;
    }

    buffer[received] = '\0';

    // Parse request line
    char method[16] = {0}, path[512] = {0}, version[16] = {0};
    if (sscanf(buffer, "%15s %511s %15s", method, path, version) != 3) {
        send_error(client_socket, 400, "Malformed request");
        free(buffer);
        close(client_socket);
        return NULL;
    }

    char log_buf[1024];
    snprintf(log_buf, sizeof(log_buf), "%s %s", method, path);
    log_msg(LOG_INFO, log_buf);

    // Find body
    char *body = strstr(buffer, "\r\n\r\n");
    size_t body_len = 0;
    if (body) {
        body += 4;
        body_len = received - (body - buffer);
    }

    // Route request
    if (strcmp(method, "GET") == 0) {
        handle_get_request(client_socket, path);
    } else if (strcmp(method, "POST") == 0) {
        handle_post_request(client_socket, path, buffer, body, body_len);
    } else if (strcmp(method, "OPTIONS") == 0) {
        handle_options_request(client_socket);
    } else {
        send_error(client_socket, 405, "Method not allowed");
    }

    free(buffer);
    close(client_socket);
    return NULL;
}

// Signal handler for graceful shutdown
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        log_msg(LOG_INFO, "Shutdown signal received");
        server_running = 0;
    }
}

int main(int argc, char *argv[]) {
    // Parse environment variables (Railway compatibility)
    char *port_env = getenv("PORT");
    if (port_env) {
        config.port = atoi(port_env);
    }

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            config.port = atoi(argv[++i]);
        }
    }

    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);  // Ignore broken pipe

    log_msg(LOG_INFO, "=== Film Negative Processor API v2.0 ===");
    char msg[256];
    snprintf(msg, sizeof(msg), "Starting server on port %d", config.port);
    log_msg(LOG_INFO, msg);

    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        log_msg(LOG_ERROR, "Failed to create socket");
        return 1;
    }

    // Set socket options
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config.port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        snprintf(msg, sizeof(msg), "Failed to bind to port %d: %s", config.port, strerror(errno));
        log_msg(LOG_ERROR, msg);
        close(server_socket);
        return 1;
    }

    // Listen
    if (listen(server_socket, config.max_connections) < 0) {
        log_msg(LOG_ERROR, "Failed to listen on socket");
        close(server_socket);
        return 1;
    }

    snprintf(msg, sizeof(msg), "Server ready at http://0.0.0.0:%d", config.port);
    log_msg(LOG_INFO, msg);
    log_msg(LOG_INFO, "Endpoints: POST /api/to-negative, POST /api/to-positive, GET /health");

    // Accept connections
    while (server_running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int *client_socket = malloc(sizeof(int));

        if (!client_socket) {
            log_msg(LOG_ERROR, "Failed to allocate memory for client socket");
            continue;
        }

        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (*client_socket < 0) {
            free(client_socket);
            if (server_running) {
                log_msg(LOG_WARN, "Failed to accept connection");
            }
            continue;
        }

        // Handle in new thread
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_socket) != 0) {
            log_msg(LOG_ERROR, "Failed to create thread");
            close(*client_socket);
            free(client_socket);
        } else {
            pthread_detach(thread);
        }
    }

    close(server_socket);
    log_msg(LOG_INFO, "Server shutdown complete");
    return 0;
}
