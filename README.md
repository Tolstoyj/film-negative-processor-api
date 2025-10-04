# 🎬 Film Negative Processor API

> Transform images into vintage film negatives with authentic effects. Production-ready REST API built in pure C.

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](VERSION)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Railway](https://img.shields.io/badge/deploy-railway-purple.svg)](https://railway.app)

![Film Negative Example](https://via.placeholder.com/800x200/ff8c00/ffffff?text=Film+Negative+Processor+API)

## ✨ Features

- 🎞️ **Authentic Film Effects** - Color inversion, grain, sprocket holes, amber cast
- 🔄 **Bidirectional Processing** - Convert to negative and back to positive
- ⚡ **High Performance** - Written in C with zero external dependencies
- 🚀 **Production Ready** - Deployed on Railway with 200+ concurrent connections
- 🔒 **Secure** - Built-in validation, timeouts, and security headers
- 📦 **Easy Deploy** - One-click Railway deployment or Docker

## 🚀 Quick Start

### Deploy to Railway (30 seconds)

[![Deploy on Railway](https://railway.app/button.svg)](https://railway.app/new)

See [docs/RAILWAY_DEPLOY.md](docs/RAILWAY_DEPLOY.md) for detailed instructions.

### Local Development

```bash
# Clone repository
git clone https://github.com/yourusername/film-processor-api.git
cd film-processor-api

# Build
make

# Run
./bin/film_server
# Server running at http://localhost:8080
```

### Docker

```bash
docker build -f Dockerfile.railway -t film-processor .
docker run -p 8080:8080 film-processor
```

## 📡 API Usage

### Convert to Film Negative

```bash
curl -X POST http://localhost:8080/api/to-negative \
  -F "image=@photo.jpg" \
  -o negative.jpg

# For Railway deployment, use HTTP/1.1 for clean transfers
curl --http1.1 -X POST https://your-app.railway.app/api/to-negative \
  -F "image=@photo.jpg" \
  -o negative.jpg
```

### Convert Back to Positive

```bash
curl -X POST http://localhost:8080/api/to-positive \
  -F "image=@negative.jpg" \
  -o restored.jpg

# For Railway deployment
curl --http1.1 -X POST https://your-app.railway.app/api/to-positive \
  -F "image=@negative.jpg" \
  -o restored.jpg
```

### Health Check

```bash
curl http://localhost:8080/health
# {"status":"healthy","service":"film-processor","version":"2.0"}
```

## 💻 Client Examples

### JavaScript

```javascript
async function convertToNegative(file) {
  const formData = new FormData();
  formData.append('image', file);

  const response = await fetch('http://localhost:8080/api/to-negative', {
    method: 'POST',
    body: formData
  });

  return await response.blob();
}
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
```

## 📁 Project Structure

```
film-processor-api/
├── src/                    # Source code
│   ├── server_v2.c        # Production API server
│   ├── film_processor.c   # Core processing library
│   └── vintage_filter.c   # CLI tool
├── include/               # Header files
│   ├── film_processor.h
│   ├── stb_image.h
│   └── stb_image_write.h
├── docs/                  # Documentation
│   ├── API.md
│   ├── RAILWAY_DEPLOY.md
│   ├── CHANGELOG.md
│   └── RELEASE_NOTES_v2.0.md
├── scripts/               # Utility scripts
│   └── test_api.sh
├── bin/                   # Compiled binaries (generated)
├── Makefile.production    # Production build
├── Dockerfile.railway     # Railway deployment
└── README.md             # This file
```

## 🛠️ Build from Source

### Requirements

- GCC 7.0+ or Clang 10.0+
- Make
- pthread support
- Linux/macOS/WSL

### Build Commands

```bash
# Production build (optimized)
make

# Debug build
make debug

# Clean
make clean

# Install to /usr/local/bin
sudo make install

# Run tests
make test
```

## 🎨 Effects Applied

### To Negative Mode
- ✨ Color inversion (true negative)
- 🎞️ Orange/amber film base cast
- 📊 Film grain texture
- 🎬 Sprocket hole borders

### To Positive Mode
- 🔄 Color restoration
- 🧹 Orange cast removal
- ✂️ Border cleanup
- 📷 Tone balancing

## 📊 Performance

| Metric | Value |
|--------|-------|
| Max Concurrent Connections | 200 |
| Max Upload Size | 20MB |
| Request Timeout | 30 seconds |
| Processing Time (600x400) | ~150ms |
| Processing Time (3000x2000) | ~500ms |
| Memory Usage | ~512MB |
| Supported Formats | JPG, PNG, BMP, TGA |

## 🔒 Security

- ✅ Request size validation (20MB limit)
- ✅ Content-Type verification
- ✅ Timeout protection (30s)
- ✅ Input sanitization
- ✅ Security headers (XSS, Frame Options, Content-Type)
- ✅ CORS enabled for web applications

## 🌐 API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/to-negative` | POST | Convert image to film negative |
| `/api/to-positive` | POST | Convert negative back to positive |
| `/health` | GET | Health check with version info |
| `/` | GET | API information |

## 📖 Documentation

- **[API Documentation](docs/API.md)** - Complete API reference
- **[Railway Deployment](docs/RAILWAY_DEPLOY.md)** - Deployment guide
- **[Changelog](docs/CHANGELOG.md)** - Version history
- **[Release Notes](docs/RELEASE_NOTES_v2.0.md)** - Latest release details

## 🧪 Testing

```bash
# Health check
curl http://localhost:8080/health

# Test with image
curl -X POST http://localhost:8080/api/to-negative \
  -F "image=@test.jpg" -o result.jpg

# Run automated tests
./scripts/test_api.sh test.jpg
```

## 🚢 Deployment

### Railway (Recommended)
1. Push to GitHub
2. Connect to Railway
3. Auto-deploys from `Dockerfile.railway`

### Docker
```bash
docker build -f Dockerfile.production -t film-api:2.0 .
docker run -d -p 8080:8080 --name film-api film-api:2.0
```

### Manual
```bash
make production
./bin/film_server --port 8080
```

## 🤝 Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **[stb_image](https://github.com/nothings/stb)** - Sean Barrett's excellent header-only image library
- **Railway** - For providing an excellent deployment platform
- **Contributors** - Everyone who has helped improve this project

## 📞 Support

- 🐛 **Issues**: [GitHub Issues](https://github.com/yourusername/film-processor-api/issues)
- 📧 **Email**: support@filmprocessor.com
- 📖 **Docs**: [docs/](docs/)

## ⭐ Show Your Support

Give a ⭐️ if this project helped you!

---

**Built with ❤️ in C** | **Version 2.0.0** | **Production Ready** ✅
