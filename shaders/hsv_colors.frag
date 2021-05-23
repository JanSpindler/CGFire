#version 330 core

in vec4 interp_color;

out vec4 frag_color;

vec4 hsva2rgba(vec4 hsva) {
	// Vincent Schönbach
    // keine Ahnung, wie es hergeleitet wurde, scheint aber zu funktionieren
    // https://de.wikipedia.org/wiki/HSV-Farbraum#Umrechnung_HSV_in_RGB
    float H = hsva.x * 360.f;
    float S = hsva.y;
    float V = hsva.z;

    int h_i = int(H/60.f);

    float f = (H/60.f) - h_i;

    float p = V*(1.f-S);
    float q = V*(1-S*f);
    float t = V*(1.f-S*(1.f-f));

    switch(h_i){
        case 0: case 6: return vec4(V, t, p, hsva.a);
        case 1: return vec4(q, V, p, hsva.a);
        case 2: return vec4(p, V, t, hsva.a);
        case 3: return vec4(p, q, V, hsva.a);
        case 4: return vec4(t, p, V, hsva.a);
        case 5: return vec4(V, p, q, hsva.a);
    }


    return hsva;
}

void main()
{
    frag_color = hsva2rgba(interp_color);
}
