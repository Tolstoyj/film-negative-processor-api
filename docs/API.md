# Film Negative Processor API v2.0

> **Production-ready REST API for converting images between normal color and vintage film negative effects**

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](VERSION)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Railway](https://img.shields.io/badge/deploy-railway-purple.svg)](https://railway.app)

## 🎯 What's New in v2.0

- ✅ **Production-Ready API** - Fully tested and battle-hardened
- ✅ **Railway Deployment** - One-click deploy with Railway
- ✅ **Enhanced Security** - Security headers, validation, and protection
- ✅ **Robust Multipart Parsing** - Handles all edge cases gracefully
- ✅ **Comprehensive Logging** - Structured logs with multiple levels
- ✅ **Graceful Shutdown** - Signal handling for clean restarts

## 🚀 Quick Start

### Option 1: Deploy to Railway (Recommended)

[![Deploy on Railway](https://railway.app/button.svg)](https://railway.app/new)

See [RAILWAY_DEPLOY.md](RAILWAY_DEPLOY.md) for detailed instructions.

### Option 2: Local Development

```bash
# Build
gcc -O2 -pthread -o film_server server_v2.c film_processor.c -lm -lpthread

# Run
./film_server
# Server starts on http://localhost:8080

# Test
curl http://localhost:8080/health
```

### Option 3: Docker

```bash
docker build -f Dockerfile.railway -t film-processor .
docker run -p 8080:8080 film-processor
```

## 📡 API Endpoints

### Convert to Negative
```bash
POST /api/to-negative
```

**Request:**
```bash
curl -X POST http://localhost:8080/api/to-negative \
  -F "image=@photo.jpg" \
  -o negative.jpg
```

**Response:** JPEG image with film negative effects

---

### Convert to Positive
```bash
POST /api/to-positive
```

**Request:**
```bash
curl -X POST http://localhost:8080/api/to-positive \
  -F "image=@negative.jpg" \
  -o restored.jpg
```

**Response:** JPEG image converted back to positive

---

### Health Check
```bash
GET /health
```

**Response:**
```json
{
  "status": "healthy",
  "service": "film-processor",
  "version": "2.0"
}
```

---

### API Info
```bash
GET /
```

**Response:**
```json
{
  "service": "Film Negative Processor",
  "version": "2.0.0",
  "endpoints": ["/api/to-negative", "/api/to-positive", "/health"]
}
```

## 💻 Client Examples

### JavaScript/TypeScript
```javascript
const API_URL = 'http://localhost:8080';

async function convertToNegative(file) {
  const formData = new FormData();
  formData.append('image', file);

  const response = await fetch(`${API_URL}/api/to-negative`, {
    method: 'POST',
    body: formData
  });

  return await response.blob();
}

// Usage in React/Vue/Svelte
<input type="file" onChange={async (e) => {
  const blob = await convertToNegative(e.target.files[0]);
  const url = URL.createObjectURL(blob);
  setImageUrl(url);
}} />
```

### Python
```python
import requests

def convert_to_negative(image_path):
    with open(image_path, 'rb') as f:
        response = requests.post(
            'http://localhost:8080/api/to-negative',
            files={'image': f}
        )
    return response.content

# Save result
with open('negative.jpg', 'wb') as f:
    f.write(convert_to_negative('photo.jpg'))
```

### cURL
```bash
# Convert to negative
curl -X POST http://localhost:8080/api/to-negative \
  -F "image=@input.jpg" \
  -o negative.jpg

# Convert back to positive
curl -X POST http://localhost:8080/api/to-positive \
  -F "image=@negative.jpg" \
  -o restored.jpg
```

## 🎨 Effects Applied

### To Negative Mode
- ✨ **Color Inversion** - True negative colors
- 🎞️ **Film Base Cast** - Authentic orange/amber tint
- 📊 **Film Grain** - Realistic texture
- 🎬 **Sprocket Holes** - Vintage film strip borders

### To Positive Mode
- 🔄 **Color Restoration** - Inverts back to original
- 🧹 **Cast Removal** - Removes orange tint
- ✂️ **Border Removal** - Cleans sprocket holes
- 📷 **Color Correction** - Balances tones

## 🏗️ Architecture

```
┌─────────────────┐
│   HTTP Client   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   server_v2.c   │  ← REST API + Multipart Parser
│   (API Layer)   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│film_processor.c │  ← Core Image Processing
│   (Library)     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   stb_image     │  ← Image Codec (header-only)
└─────────────────┘
```

## 📊 Performance

- **Concurrent Requests**: 200 simultaneous connections
- **Max Upload Size**: 20MB
- **Request Timeout**: 30 seconds
- **Memory**: ~512MB recommended
- **CPU**: Optimized with -O2 flag
- **Processing Time**: ~100-500ms per image (depends on size)

## 🔒 Security

### Built-in Protection
- ✅ Request size validation (20MB limit)
- ✅ Content-Type verification
- ✅ Timeout protection (30s)
- ✅ Memory allocation guards
- ✅ Input sanitization

### Security Headers
```
X-Content-Type-Options: nosniff
X-Frame-Options: DENY
X-XSS-Protection: 1; mode=block
Access-Control-Allow-Origin: *
```

## 📝 Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | 8080 | Server port (Railway sets this automatically) |

## 🐛 Troubleshooting

### Server Won't Start
```bash
# Check port availability
lsof -i :8080

# Check logs
tail -f server_v2.log
```

### Image Upload Fails
- Verify file size < 20MB
- Check Content-Type is `multipart/form-data`
- Ensure image format is JPG/PNG/BMP/TGA

### Connection Reset
- Check server logs for errors
- Verify multipart boundary format
- Try smaller test image first

## 📈 Deployment

### Railway (Recommended)
1. Push to GitHub
2. Connect to Railway
3. Deploy automatically

See [RAILWAY_DEPLOY.md](RAILWAY_DEPLOY.md)

### Docker
```bash
docker build -f Dockerfile.railway -t film-api .
docker run -d -p 8080:8080 film-api
```

### Manual
```bash
gcc -O2 -pthread -o film_server server_v2.c film_processor.c -lm -lpthread
./film_server --port 8080
```

## 🧪 Testing

### Health Check
```bash
curl http://localhost:8080/health
# Expected: {"status":"healthy",...}
```

### Full Workflow Test
```bash
# 1. Convert to negative
curl -X POST http://localhost:8080/api/to-negative \
  -F "image=@test.jpg" -o negative.jpg

# 2. Convert back to positive
curl -X POST http://localhost:8080/api/to-positive \
  -F "image=@negative.jpg" -o restored.jpg
```

## 📚 Documentation

- [Railway Deployment Guide](RAILWAY_DEPLOY.md)
- [Changelog](CHANGELOG.md)
- [API Documentation](README_v2.md) (this file)

## 🛠️ Tech Stack

- **Language**: C (C99)
- **Concurrency**: POSIX Threads
- **Libraries**: stb_image (header-only)
- **Protocol**: HTTP/1.1
- **Parsing**: Custom multipart/form-data parser

## 📄 License

MIT License - See LICENSE file

## 🤝 Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create feature branch
3. Make your changes
4. Submit pull request

## 📞 Support

- 🐛 **Issues**: [GitHub Issues](https://github.com/yourusername/film-processor/issues)
- 📧 **Email**: support@yourproject.com
- 💬 **Discord**: [Join Server](https://discord.gg/yourserver)

## ⭐ Show Your Support

If this project helped you, please give it a ⭐️!

---

**Version**: 2.0.0
**Status**: Production Ready ✅
**Last Updated**: 2025-10-04
