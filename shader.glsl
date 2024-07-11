#version 330

in vec2 fragTexCoord;

uniform int maxiter;
uniform vec2 x;
uniform vec2 y;

out vec4 finalColor;

void main() {
    float cr = x.x + fragTexCoord.x * (x.y - x.x);
    float ci = y.x + fragTexCoord.y * (y.y - y.x);

    float zr = 0.0;
    float zi = 0.0;

    for (float i = 0; i < maxiter; ++i) {
        float nzr = zr * zr - zi * zi + cr;
        float nzi = 2.0 * zr * zi + ci;

        zr = nzr;
        zi = nzi;

        if (zr * zr + zi * zi > 4.0) {
            float c = i / maxiter;
            finalColor = vec4(c, c, c, 1.0);
            return;
        }
    }

    finalColor = vec4(0.0, 0.0, 0.0, 1.0);
}
