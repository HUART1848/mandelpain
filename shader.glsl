#version 330

in vec2 fragTexCoord;

uniform int palette;
uniform int maxiter;
uniform vec2 x;
uniform vec2 y;

out vec4 finalColor;

vec2 cpow2(vec2 c) {
    return vec2(
        c.x * c.x - c.y * c.y,
        2.0 * c.x * c.y        
    );
}

vec3 bwpalette(int i) {
    float c = (i % 64) / 64.0;
    return vec3(c, c, c);
}

vec3 rgbpalette(int i) {
    return vec3(
        (i % 32) / 32.0,
        (i % 64) / 64.0,
        (i % 96) / 96.0
    );
}

void main() {
    float cr = x.x + fragTexCoord.x * (x.y - x.x);
    float ci = y.x + (1.0 - fragTexCoord.y) * (y.y - y.x);
    
    float zr = 0.0;
    float zi = 0.0;

    for (int i = 0; i < maxiter; ++i) {
        vec2 z = cpow2(vec2(zr, zi));

        zr = z.x + cr;
        zi = z.y + ci;

        if (zr * zr + zi * zi > 4.0) {
            if (palette == 0) {
                finalColor = vec4(bwpalette(i), 1.0);
            } else {
                finalColor = vec4(rgbpalette(i), 1.0);
            }
            return;
        }
    }

    finalColor = vec4(0.0, 0.0, 0.0, 1.0);
}
