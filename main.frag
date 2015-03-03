// ........................phong.frag ...........................
// Phong lighting in eye coordinates.

// These are set by the .vert code, and they're interpolated.
varying vec3 ec_vnormal, ec_vposition;

void main()
{
    vec3 P, N, L, V, H;
    vec4 diffuse_color = gl_FrontMaterial.diffuse;
    vec4 specular_color = gl_FrontMaterial.specular; 
    float shininess = gl_FrontMaterial.shininess;

    vec4 finalColor = vec4(0.0,0.0,0.0,0.0);

    P = ec_vposition;
    N = normalize(ec_vnormal);
    V = normalize(-P);

    for (int i=0;i<3;i++) 
    {
    	L = normalize(vec3(gl_LightSource[i].position) - P);
    	H = normalize(L+V);

    	vec4 Lamb = gl_FrontLightProduct[i].ambient;

    	vec4 Ldiff = gl_FrontLightProduct[i].diffuse * max(dot(N,L),0.0);
    	Ldiff = clamp(Idiff, 0.0, 1.0); 
   
    	vec4 Lspec = gl_FrontLightProduct[i].specular * pow(max(dot(H,N),0.0),shininess);
    	Lspec = clamp(Ispec, 0.0, 1.0); 
   
    	finalColor += Lamb + Ldiff + Lspec;
    }

    gl_FragColor = finalColor;
}
