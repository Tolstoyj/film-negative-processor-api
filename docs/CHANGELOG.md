# Changelog

## [2.0.0] - 2025-10-04

### 🚀 Production Release - Railway Ready

### Added
- **Production-ready HTTP API Server** (server_v2.c)
  - Robust multipart/form-data parsing with fallback strategies
  - Support for Railway deployment with automatic PORT detection
  - Comprehensive error handling and validation
  - Request timeout protection (30s)
  - Graceful shutdown with signal handling (SIGINT, SIGTERM)

- **Enhanced Security**
  - Security headers (X-Content-Type-Options, X-Frame-Options, X-XSS-Protection)
  - Request size limits (20MB max)
  - Content-Type validation
  - Memory allocation safeguards

- **Advanced Logging**
  - Log levels: DEBUG, INFO, WARN, ERROR
  - Timestamped structured logs
  - Request/response tracking
  - Performance monitoring ready

- **Railway Deployment**
  - `Dockerfile.railway` - Optimized for Railway platform
  - `railway.json` - Railway configuration
  - `.railwayignore` - Exclude unnecessary files
  - `RAILWAY_DEPLOY.md` - Complete deployment guide

- **API Endpoints**
  - `POST /api/to-negative` - Convert image to film negative
  - `POST /api/to-positive` - Convert negative back to positive
  - `GET /health` - Health check with version info
  - `GET /` - API information and documentation
  - CORS enabled for web applications

- **Documentation**
  - Complete Railway deployment guide
  - API usage examples (cURL, JavaScript, Python)
  - Troubleshooting section
  - Production scaling guidelines

### Improved
- **Multipart Parser** - More robust boundary detection
  - Handles multiple boundary formats
  - Graceful degradation with fallback strategies
  - Better error messages for debugging

- **Memory Management**
  - Proper cleanup of temporary files
  - Resource leak prevention
  - Thread-safe operations

- **Error Handling**
  - JSON error responses with timestamps
  - HTTP status codes aligned with errors
  - Detailed error messages for debugging

### Fixed
- Connection reset issues during image upload
- Multipart boundary parsing edge cases
- Memory leaks in error paths
- Port binding conflicts

### Technical Details
- **Language**: C (C99 standard)
- **Concurrency**: POSIX threads (pthread)
- **Max Concurrent**: 200 connections
- **Max Upload Size**: 20MB
- **Supported Formats**: JPG, PNG, BMP, TGA
- **Output Format**: JPEG (90% quality)

---

## [1.0.0] - 2025-10-04

### Initial Release

### Added
- Core film negative processing engine
- Command-line tool (`vintage_filter`)
- Basic HTTP server implementation
- Docker support
- Comprehensive documentation

### Features
- **Film Negative Effects**
  - Color inversion
  - Orange/amber film base color cast
  - Film grain texture
  - Sprocket hole borders

- **Reverse Processing**
  - Negative to positive conversion
  - Color cast removal
  - Border removal

- **Image Processing**
  - Support for JPG, PNG, BMP, TGA formats
  - Handles images up to 3000x2000px+
  - RK4-based quality preservation

### Known Issues
- HTTP server multipart parsing incomplete (v1.0)
- Fixed in v2.0

---

## Version History
- **v2.0.0** - Production-ready API with Railway deployment (Current)
- **v1.0.0** - Initial release with CLI tool
