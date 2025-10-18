#version 330 core	     // Minimal GL version support expected from the GPU

// in vec3 fNormal;
// out vec4 color;

// void main() {
//     color = vec4(normalize(fNormal), 1.0);
// }

// uniform vec3 camPos;
// in vec3 fPosition;
// in vec3 fNormal;
// out vec4 color;
// void main(){

// float ka = 0.3;     // Ambient
// float kd = 1.6;     // Diffuse
// float ks = 1.0;
// float shininess = 32.0;


// const vec3 ambientsurfacecolor=vec3(1.0,0,0) ;
// const vec3 diffusesurfacecolor=vec3(1.0,0.0,0) ;
// const vec3 specularsurfacecolor = vec3(1.0, 1.0, 1.0);



// const vec3 ambientlightcolor = vec3(1.0,1.0,1.0) ;
// const vec3 specularlightcolor = vec3(1.0,1.0,1.0);
// const vec3 diffuselightcolor =vec3(1.0,1.0,1.0);


// vec3 n = normalize(fNormal);
// vec3 l = normalize(vec3(1.0, 1.0, 0.0)); // light direction vector (hard-coded just for now)
// vec3 v = normalize(camPos - fPosition);
// vec3 r = reflect(-l, n);
// vec3 ambient = ka*ambientsurfacecolor*ambientlightcolor;//red
// vec3 diffuse = kd * max(dot(n,l),0) * diffusesurfacecolor * diffuselightcolor;
// vec3 specular =ks*pow(max(dot(v,r),0),shininess)*specularsurfacecolor*specularlightcolor;
// color = vec4(ambient + diffuse + specular, 1.0); // Building RGBA from RGB.
// }


uniform vec3 camPos;
uniform vec3 objectColor;
uniform int isLightSource;

in vec3 fPosition;
in vec3 fNormal;

out vec4 color;

void main()
{

    float ka = 0.3;
    float kd = 1.0;
    float ks = 0.6;
    float alpha = 64.0;
    vec3 lightColor = vec3(1.0);

    if (isLightSource == 1) {
        // Le soleil s’éclaire lui
        color = vec4(objectColor, 1.0);
        return;
    }

    vec3 n = normalize(fNormal);
    vec3 lightPos = vec3(0.0, 0.0, 0.0); // soleil au centre
    vec3 l = normalize(lightPos - fPosition);
    vec3 v = normalize(camPos - fPosition);
    vec3 r = reflect(-l, n);


    // Composantes


    vec3 ambient  = ka * lightColor * objectColor;
    float diff = max(dot(n, l), 0.0);
    vec3 diffuse  = kd * diff * lightColor * objectColor;
    float spec = pow(max(dot(v, r), 0.0), alpha);
    vec3 specular = ks * spec * lightColor;

    vec3 finalColor = ambient + diffuse + specular;
    color = vec4(finalColor, 1.0);
}

