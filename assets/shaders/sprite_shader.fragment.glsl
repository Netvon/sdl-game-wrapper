#version 430
out vec4 FragColor;

in vec2 TexCoord;
in vec3 out_color;

uniform sampler2D texture1;

void main() {
	vec4 col = texture(texture1, TexCoord);
	if(col.a < 0.01) {
		discard;
	}

	FragColor = vec4(out_color, 1.0) * col;
}