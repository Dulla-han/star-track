#version 330

//components for each material
struct Material {
  vec3  ambient;
  vec3  diffuse;
  vec3  specular;
  float shininess;
  bool  useTexture;
};

//use material as a uniform
uniform Material material;
uniform mat4 view;
uniform sampler2D texSampler;

//in world cordinates
//day/night parametr
uniform float sunParam;
//tower spot light
uniform vec3 spotLightPosition;
uniform vec3 spotLightDirection;
uniform vec3 spotColor;
//artificial ambient/directional sun/light
uniform vec3 globalAmbient;
uniform vec3 sunDirection;
uniform vec3 sunColor;
//point light
uniform vec3 pointLightPosition;
uniform vec3 pointLightColor;
uniform vec3 pointLightAttenuation; //constant, linear, quadratic
//attenuation = útlum (D1 vocab)

//fog uniforms
uniform float fogDensity;
uniform vec3 fogColor;

//water texture speed
uniform float waterTex;

//spriteSheet film
uniform vec4 spriteSheet;

uniform float timeElapsed;

smooth in vec2 texCoord_v;
smooth in vec3 pos_v;
smooth in vec3 N;
in float fogDistance;

out vec4 FragColor;

vec3 directionalLight(vec3 dirWorld, vec3 color, vec3 N, vec3 V) {
    //light diretion vector
    vec3 L = normalize(mat3(view) * normalize(dirWorld));

    float diff = max(dot(N, L), 0.0);
    //diffuse component of light
    vec3 diffuse = material.diffuse * color * diff;

    float spec = 0.0;
    if (diff > 0.0) {
        //reflection of -L about normal of the surface
        vec3 R = reflect(-L, N);
        //specular intensity: max of dot product of View, Reclection and shininess of material
        spec = pow(max(dot(V, R), 0.0), material.shininess);
    }

    //the actual specular component
    vec3 specular = material.specular * color * spec;

    return diffuse + specular;
}

//spotlight: position + direction with fall off
//comments pretty similar to the directionalLight() func
//won't copy paste multiple times
vec3 spotLight(vec3 posWorld, vec3 dirWorld, vec3 color, float degrCut, float exponent, vec3 pos_v, vec3 N, vec3 V) {
    vec3 lightPos_v = vec3(view * vec4(posWorld, 1.0));
    vec3 spotDir_v = normalize(mat3(view) * dirWorld);

    vec3 L = normalize(lightPos_v - pos_v);

    float diff = max(dot(N, L), 0.0); 
    float spec = 0.0f;
    if (diff > 0.0) {
        vec3 R = reflect(-L, N);
        spec = pow(max(dot(V, R), 0.0), material.shininess);
    }

    //cone falloff: fragments outside the cone get 0
    vec3 lightToVertex = normalize(pos_v - lightPos_v);
    float spotEffect = dot(lightToVertex, spotDir_v);
    float intensity = (spotEffect > cos(radians(degrCut))) ? pow(spotEffect, exponent) : 0.0;

    vec3 diffuse  = material.diffuse  * color * diff * intensity;
    vec3 specular = material.specular * color * spec * intensity;

    return diffuse + specular;
}

vec3 pointLight(vec3 lightPosWorld, vec3 color, vec3 atten, vec3 pos_v, vec3 N, vec3 V) {
    //in view coordanites
    vec3 lightPos_v = vec3(view * vec4(lightPosWorld, 1.0f));

    //direction to light
    vec3 toLight = lightPos_v - pos_v;
    float d = length(toLight);
    //normalize
    vec3 L = toLight / d;

    //attenuationFactor = 𝟏.𝟎/(𝒌𝑪 + 𝒌𝑳 ∗ 𝒅 + 𝒌𝑸 ∗ 𝒅𝟐)
    //𝒅 = distance between the light's position and the vertex
    //𝒌𝑪 = constant attenuation
    //𝒌𝑳 = linear attenuation
    //𝒌𝑸 = quadratic attenuation

    float attenuation = 1.0 / (atten.x + atten.y * d + atten.z * d * d);

    float diff = max(dot(N, L), 0.0);
    float spec = 0.0f;
    if (diff > 0.0) {
        vec3 R = reflect(-L, N);
        spec = pow(max(dot(V, R), 0.0), material.shininess);
    }

    vec3 diffuse = material.diffuse * color * diff;
    vec3 specular = material.specular * color * spec;

    //should be attenuationFactor * (ambientreflected + diffusereflected + specularreflected)
    //missing ambient component oh well
    return (diffuse + specular) * attenuation;
}

void main() {
    //re-normalize after interpolation
    vec3 Nn = normalize(N);
    //view direction (from the surface torward the camera)
    vec3 V  = normalize(-pos_v);

    //white color for now, degree and spot drop off exponent (spot light)
    float degree = 60.0f;
    float spotExponent = 1.5f;

    //calculate ambient for all + directionalLight + spotLight + pointLight
    vec3 ambient = material.ambient * globalAmbient;
    vec3 sun = directionalLight(sunDirection, sunColor, Nn, V);
    
    //add the pulse effect to the spotLight above car
    float pulse = 0.5 + 0.5 * sin(timeElapsed * 4.0);
    vec3 spot = spotLight(spotLightPosition, spotLightDirection, spotColor * pulse, degree, spotExponent, pos_v, Nn, V);
    vec3 point = pointLight(pointLightPosition, pointLightColor, pointLightAttenuation, pos_v, Nn, V);

    vec3 lit = clamp(ambient + sunParam * sun + spot + point, 0.0, 1.0);

    vec4 base;

    if (material.useTexture) {
        vec2 uv = texCoord_v;
        if (spriteSheet.x > 0.0) {
            float cols = spriteSheet.x;
            float rows = spriteSheet.y;
            float fps = spriteSheet.z;
            float totalFrames = spriteSheet.w;

            float frame = floor(mod(timeElapsed * fps, totalFrames));
            float frameX = mod(frame, cols);
            float frameY = floor(frame/ cols);

            frameY = (rows - 1.0) - frameY;

            vec2 localUV = vec2(texCoord_v.x, 1.0 - texCoord_v.y);
            uv = (localUV + vec2(frameX, frameY)) / vec2(cols, rows);
        } else if (waterTex > 0.0) {
            float t = timeElapsed * waterTex;
            mat3 translate = mat3(1.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0,
                                  t, 0.0, 1.0);
            mat3 wave = mat3(1.0, 0.0, 0.0,
                            0.0, 1.0, 0.0,
                            0.0, sin(timeElapsed * 0.7 + texCoord_v.x * 6.28) * 0.1, 1.0);
            mat3 uvTransform = translate * wave;
            vec3 uvH = uvTransform * vec3(texCoord_v, 1.0);
            uv = uvH.xy;
        }
        vec4 texColor = texture(texSampler, uv);
        base = vec4(lit * texColor.rgb, texColor.a);
    }
    else {
        base = vec4(lit, 1.0);
    }

    float fogFactor = clamp(exp(-fogDensity * fogDistance), 0.0, 1.0);
    FragColor = vec4(mix(fogColor, base.rgb, fogFactor), base.a);
}
