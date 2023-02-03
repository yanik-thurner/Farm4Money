#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D screenTexture;
uniform sampler2D normTexture;


const float offset = 1.0 / 300.0;

mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);


void main()
{ 
    /*
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(normTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    //vec3 diffuse = col;
    */

    vec3 diffuse = texture(screenTexture, TexCoords.st).rgb;
    mat3 I;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            vec3 samplex = texelFetch(normTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(samplex); 
        }
    }

    float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
    float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

    float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));

    // Try different values and see what happens
    g = smoothstep(0.4, 0.6, g);

    vec3 edgeColor = vec3(0., 0., 0.);
    color = vec4(mix(diffuse, edgeColor, g), 1.);

    //color = vec4(diffuse - vec3(g), 1.0);
    //color = texture2D(screenTexture, TexCoords);
    //color = texture2D(normTexture, TexCoords);
    //color = vec4(texNormal, 1.0);
    //color = vec4(col, 1.0);
} 
