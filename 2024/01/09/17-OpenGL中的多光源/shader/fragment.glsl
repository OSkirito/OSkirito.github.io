#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    sampler2D diffusemap;
    sampler2D specularmap;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct DirLight {
    vec3 direction;
    vec3 lightColor;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    bool enable;
};  
uniform DirLight dirLight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight {
    vec3 lightPos;
    vec3 lightColor;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float constant;
    float linear;
    float quadratic;
    bool enable;
};  
uniform PointLight pointLight;
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct SpotLight {
    vec3 lightPos;
    vec3  lightDirection;
    float cutOff;
    float outerCutOff;
    vec3 lightColor;
    sampler2D tex;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    bool enable;
    bool usetex;
};
uniform SpotLight spotLight;
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    result += CalcPointLight(pointLight, norm, FragPos, viewDir);    
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 result = vec3(0.0f);
    if (light.enable)
    {
        vec3 lightDir = normalize(-light.direction);
        vec3 ambient  = vec3(light.ambientStrength) * light.lightColor * material.ambient * vec3(texture(material.diffusemap, TexCoords));
        
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse  = vec3(light.diffuseStrength) * light.lightColor * diff * material.diffuse * vec3(texture(material.diffusemap, TexCoords));

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = vec3(light.specularStrength) * light.lightColor * spec * material.specular * vec3(texture(material.specularmap, TexCoords));
        result = ambient + diffuse + specular;
    }
    return (result);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 result = vec3(0.0f);
    if (light.enable)
    {
        vec3 ambient = vec3(light.ambientStrength) * light.lightColor * material.ambient * vec3(texture(material.diffusemap, TexCoords));

        vec3 lightDir = normalize(light.lightPos - fragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = vec3(light.diffuseStrength) * light.lightColor * diff * material.diffuse * vec3(texture(material.diffusemap, TexCoords));

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = vec3(light.specularStrength) * light.lightColor * spec * material.specular * vec3(texture(material.specularmap, TexCoords));

        float distance    = length(light.lightPos - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

        result = ambient + diffuse + specular;
    }
    return (result);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 result = vec3(0.0f);
    if (light.enable)
    {
        vec3 ambient = vec3(light.ambientStrength) * light.lightColor * material.ambient * vec3(texture(material.diffusemap, TexCoords));

        vec3 lightDir = normalize(light.lightPos - fragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = vec3(light.diffuseStrength) * light.lightColor * diff * material.diffuse * vec3(texture(material.diffusemap, TexCoords));

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = vec3(light.specularStrength) * light.lightColor * spec * material.specular * vec3(texture(material.specularmap, TexCoords));

        float theta     = dot(lightDir, normalize(-light.lightDirection));
        float epsilon   = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        diffuse  *= intensity;
        specular *= intensity;

        result = ambient + diffuse + specular;
    }

    if (light.usetex)
    {
        result = result * vec3(texture(light.tex, vec2(viewDir.x, viewDir.y)));
    }
    return (result);
}