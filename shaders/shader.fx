Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

RWTexture2D<float3> greyscaleTexture : register(u1);
RWTexture2D<float> magnitudeTexture : register(u2); 

float2 texelSize = 1.0f / float2(1920, 1080);


float4 reyscalePass(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET {

    return tex.Sample(samplerState, texCoord); // Sample 
}


float4 greyscalePass(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET {

    float3 input = tex.Sample(samplerState, uv).rgb;
    
    float2 posCenter = uv;
    
    // Converts to greyscale using dot product with specified weights. These seemingly random numbers are for human eyes.
    float greyscale = dot(input, float3(0.299, 0.587, 0.114)); 
    uint2 pixelCoord = uint2(uv * float2(1920, 1080));
    float3 grayColor = float3(greyscale, greyscale, greyscale);

    greyscaleTexture[pixelCoord] = greyscale;
    return float4(greyscaleTexture[pixelCoord].rgb, 1.0); 

}

float4 blurPass(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET {
    
    uint2 offsetCoord = 0;
    uint2 pixelCoord = uint2(uv * float2(1920, 1080));

    float3 blurSum = float3(0.0, 0.0, 0.0);
    
    float kernelWeightSum = 1003.0;  // sum of all Gaussian kernel weights

    float gaussianKernel[49] = {
        1,  4,  7,  9,  7,  4,  1,
        4, 16, 26, 35, 26, 16,  4,
        7, 26, 41, 52, 41, 26,  7,
        9, 35, 52, 64, 52, 35,  9,
        7, 26, 41, 52, 41, 26,  7,
        4, 16, 26, 35, 26, 16,  4,
        1,  4,  7,  9,  7,  4,  1
    };

   // loop over 7x7 kernel
    for (int i = -3; i <= 3; i++) {
        
        for (int j = -3; j <= 3; j++) {

            // get neighbouring pixels
            int2 offset = int2(i, j); 
            int2 neighborCoord = int2(pixelCoord) + offset;

            // ensure the neighbor coordinate stays within the texture bounds
            neighborCoord = clamp(neighborCoord, int2(0, 0), int2(1920 - 1, 1080 - 1));

            float3 neighborColor = greyscaleTexture[neighborCoord].rgb;

            // index = i * width + j
            // ex: shifts [-3, 3] -> [0, 6]
            int kernelIndex = (i + 3) * 7 + (j + 3);
            blurSum += neighborColor * gaussianKernel[kernelIndex]; 
        }
    }
    // normalize the final blur color by dividing by the total weight sum
    blurSum /= kernelWeightSum;

    magnitudeTexture[pixelCoord] = blurSum;
    return float4(1.0, 1.0, 1.0, 1.0);
}



float4 applySobel(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET {

    float2 texelSize = 1.0f / float2(1920, 1080);

    float3 sobelX[3] = {
        float3(-1, 0, 1),
        float3(-2, 0, 2),
        float3(-1, 0, 1)
    };

    // notice sobelY is a rotation of sobelX
    float3 sobelY[3] = {
        float3(-1, -2, -1),
        float3(0, 0, 0),
        float3(1, 2, 1)
    };

    float conX = 0.0;
    float conY = 0.0;

    // Compute Sobel convolution and calculate magnnitude
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            float2 offsetUV = uv + float2(i, j) * texelSize;
            int2 tc = int2(offsetUV * float2(1920, 1080));

            float pixelM = magnitudeTexture.Load(int3(tc, 0));

            conX += pixelM * sobelX[i + 1][j + 1]; // + 1 since sobelX[-1] is bad...
            conY += pixelM * sobelY[i + 1][j + 1];
        }
    }
    float magnitude = sqrt(conX * conX + conY * conY);

    // use threshold to detect if something is an edge, make this adjustable later...
    float threshold = 0.1f;
    float maxThreshold = 0.2f;
    if (magnitude > threshold && magnitude < maxThreshold) {

        return float4(0.0, 0.0, 0.0, 1.0);  // edge is detected, return black
    } 
    else {
        
        float4 originalColor = tex.Sample(samplerState, uv);
        return originalColor;
    }
}