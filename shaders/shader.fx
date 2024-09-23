Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

RWTexture2D<float3> greyscaleTexture : register(u1);
RWTexture2D<float> magnitudeTexture : register(u2);

float2 texelSize = 1.0f / float2(1920, 1080);
int threshold = 350;
int maxThreshold = 1000;


float4 reyscalePass(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET {

    return tex.Sample(samplerState, texCoord); // Sample 
}


float4 greyscalePass(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET {

    float3 input = tex.Sample(samplerState, uv).rgb;

    //uint2 pixelCoord = uint2(uv * float2(BUFFER_WIDTH, BUFFER_HEIGHT)); // ReShade semantics.
    
    float2 posCenter = uv;
    //float centerDepth = ReShade::GetLinearizedDepth(posCenter);
    
    // Converts to greyscale using dot product with specified weights. These seemingly random numbers are for human eyes.
    float greyscale = dot(input, float3(0.299, 0.587, 0.114)); 
    uint2 pixelCoord = uint2(uv * float2(1920, 1080));
    float3 grayColor = float3(greyscale, greyscale, greyscale);

    greyscaleTexture[pixelCoord] = greyscale;
    return float4(grayColor, 1.0);

}

float4 blurPass(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET {
    
    uint2 offsetCoord = 0;
    uint2 pixelCoord = uint2(uv * float2(1920, 1080));
    float2 texelSize = 1.0f / float2(1920, 1080);

    float3 blurSum = float3(0.0, 0.0, 0.0);
    
    float kernelWeightSum = 1003.0;  // Sum of all Gaussian kernel weights

    float gaussianKernel[49] = {
        1,  4,  7,  9,  7,  4,  1,
        4, 16, 26, 35, 26, 16,  4,
        7, 26, 41, 52, 41, 26,  7,
        9, 35, 52, 64, 52, 35,  9,
        7, 26, 41, 52, 41, 26,  7,
        4, 16, 26, 35, 26, 16,  4,
        1,  4,  7,  9,  7,  4,  1
    };

    // Loop over the 7x7 Gaussian kernel
    for (int i = -3; i <= 3; i++) {
        for (int j = -3; j <= 3; j++) {

            float2 offset = float2(i, j) * texelSize;  // Calculate texel offset
            //float3 neighborColor = greyscaleTexture.Sample(samplerState, uv + offset).rgb;  // Sample neighbor\
            offsetCoord = pixelCoord + uint2(i, j);
            float3 neighborColor = greyscaleTexture[offsetCoord].rgb;

            // Access the flattened 1D Gaussian kernel array
            int kernelIndex = (i + 3) * 7 + (j + 3);
            blurSum += neighborColor * gaussianKernel[kernelIndex];  // Apply Gaussian weight
        }
    }

    // Normalize the final blur color by dividing by the total weight sum
    blurSum /= kernelWeightSum;

    magnitudeTexture[uint2(uv * float2(1920, 1080))] = blurSum;
    return float4(blurSum, 1.0);  // Return blurred color with full opacity
}

float4 applySobel(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET {

    float3 sobelX[3] = {
        float3(-1, 0, 1),
        float3(-2, 0, 2),
        float3(-1, 0, 1)
    };

    float3 sobelY[3] = {
        float3(-1, -2, -1),
        float3(0, 0, 0),
        float3(1, 2, 1)
    };

    float conX = 0.0;
    float conY = 0.0;

    // Compute Sobel convolution
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {

            float2 offset = float2(i, j) * texelSize;
            float3 neighborColor = tex.Sample(samplerState, uv + offset).rgb;

            conX += neighborColor.r * sobelX[i + 1][j + 1];
            conY += neighborColor.r * sobelY[i + 1][j + 1];
        }
    }

    // Calculate magnitude
    float magnitude = sqrt(conX * conX + conY * conY);
    //magnitude = magnitude / 30.0;   Normalize the magnitude for thresholding
    magnitude *= 30.0;
    // Apply threshold-based edge detection
    if (magnitude > threshold && magnitude < maxThreshold) {

        return float4(0.0, 0.0, 0.0, 1.0);  // Edge is detected, return black
    } else {
    
        float4 originalColor = tex.Sample(samplerState, uv);  // Otherwise, return original color
        return originalColor;
    }
}