# Railway Deployment Guide - Film Negative Processor API v2.0

## 🚀 Quick Deploy to Railway

### Prerequisites
- Railway account (https://railway.app)
- GitHub repository (optional but recommended)
- Railway CLI installed (optional)

## Method 1: Deploy from GitHub (Recommended)

### Step 1: Push to GitHub
```bash
git init
git add .
git commit -m "Initial commit - Film Processor API v2.0"
git remote add origin https://github.com/yourusername/film-processor-api.git
git push -u origin main
```

### Step 2: Deploy on Railway
1. Go to https://railway.app
2. Click "New Project"
3. Select "Deploy from GitHub repo"
4. Choose your repository
5. Railway will auto-detect the Dockerfile and deploy

### Step 3: Environment Variables (Optional)
Railway automatically sets the `PORT` environment variable. No additional configuration needed!

## Method 2: Deploy with Railway CLI

### Step 1: Install Railway CLI
```bash
npm install -g @railway/cli
```

### Step 2: Login
```bash
railway login
```

### Step 3: Initialize and Deploy
```bash
railway init
railway up
```

### Step 4: Get Your URL
```bash
railway domain
```

## Method 3: Deploy with Dockerfile

Railway will automatically detect `Dockerfile.railway` and build your application.

## 📋 What Gets Deployed

### Files Included:
- `server_v2.c` - Production API server
- `film_processor.c` - Core processing library
- `film_processor.h` - Library header
- `stb_image.h` - Image loading
- `stb_image_write.h` - Image encoding
- `Dockerfile.railway` - Railway-optimized build

### Build Process:
1. Railway uses `gcc:latest` base image
2. Compiles C code with optimizations (`-O2`)
3. Creates `/tmp` directory for file operations
4. Exposes port from `PORT` environment variable
5. Sets up health checks

## 🔧 Configuration

### Environment Variables (Auto-configured by Railway)
- `PORT` - Automatically set by Railway (typically 8080 or random)

### Custom Configuration (Optional)
You can override settings in Railway dashboard:
- **Restart Policy**: ON_FAILURE (default)
- **Replicas**: 1 (can scale up)
- **Health Check**: Enabled at `/health`

## 📊 Monitoring

### Health Check Endpoint
```bash
curl https://your-app.railway.app/health
```

**Expected Response:**
```json
{
  "status": "healthy",
  "service": "film-processor",
  "version": "2.0"
}
```

### API Endpoints
- `GET /` - API information
- `GET /health` - Health check
- `POST /api/to-negative` - Convert to negative
- `POST /api/to-positive` - Convert to positive

### Logs
View logs in Railway dashboard or via CLI:
```bash
railway logs
```

## 🧪 Testing Your Deployment

### Test Health Check
```bash
curl https://your-app.railway.app/health
```

### Test Image Conversion
```bash
# Convert to negative
curl -X POST https://your-app.railway.app/api/to-negative \
  -F "image=@photo.jpg" \
  -o negative.jpg

# Convert back to positive
curl -X POST https://your-app.railway.app/api/to-positive \
  -F "image=@negative.jpg" \
  -o restored.jpg
```

### JavaScript/Frontend Test
```javascript
const API_URL = 'https://your-app.railway.app';

async function convertImage(file, mode) {
  const formData = new FormData();
  formData.append('image', file);

  const endpoint = mode === 'negative' ? '/api/to-negative' : '/api/to-positive';

  const response = await fetch(API_URL + endpoint, {
    method: 'POST',
    body: formData
  });

  return await response.blob();
}
```

## 📈 Performance & Limits

### Railway Free Tier:
- 500 hours/month execution time
- 512MB RAM
- 1GB disk space
- Shared CPU

### Application Limits:
- Max upload size: 20MB
- Max concurrent requests: 200
- Request timeout: 30 seconds
- Supported formats: JPG, PNG, BMP, TGA

## 🔒 Security Features

### Built-in Security Headers:
- `X-Content-Type-Options: nosniff`
- `X-Frame-Options: DENY`
- `X-XSS-Protection: 1; mode=block`
- CORS enabled with `Access-Control-Allow-Origin: *`

### Request Validation:
- Content-Type verification
- Size limit enforcement (20MB)
- Multipart boundary validation
- Timeout protection (30s)

## 🐛 Troubleshooting

### Build Fails
- Check Railway build logs
- Verify all source files are present
- Ensure Dockerfile.railway is in root directory

### Server Won't Start
- Check `PORT` environment variable is set
- Review server logs: `railway logs`
- Verify health check endpoint responds

### Image Upload Fails
- Check request size (<20MB)
- Verify Content-Type is `multipart/form-data`
- Ensure image format is supported

### Connection Reset Errors
- Check Railway logs for server errors
- Verify multipart boundary is correct
- Try smaller image sizes

## 🔄 Updates & Redeployment

### Auto-deploy from GitHub
Railway automatically redeploys when you push to main branch.

### Manual Redeploy
```bash
railway up
```

### Rollback
Use Railway dashboard to rollback to previous deployment.

## 💰 Scaling & Production

### Horizontal Scaling
```bash
# In Railway dashboard
Settings → Replicas → Set to desired count
```

### Vertical Scaling
Upgrade Railway plan for:
- More RAM (up to 32GB)
- More CPU
- Higher concurrent connections

### Production Checklist
- [ ] Custom domain configured
- [ ] Error monitoring enabled
- [ ] Log aggregation set up
- [ ] Backup strategy defined
- [ ] Load testing completed
- [ ] Rate limiting implemented (if needed)

## 📞 Support

### Railway Support
- Docs: https://docs.railway.app
- Discord: https://discord.gg/railway
- Twitter: @Railway

### Application Support
- GitHub Issues: https://github.com/yourusername/film-processor-api/issues
- Documentation: See README.md

## 🎉 Success!

Your Film Negative Processor API is now live on Railway!

**Next Steps:**
1. Test all endpoints
2. Share your API URL with users
3. Monitor logs and performance
4. Consider adding rate limiting for production use
5. Set up custom domain (optional)

---

**Deployment Version:** 2.0.0
**Last Updated:** 2025-10-04
