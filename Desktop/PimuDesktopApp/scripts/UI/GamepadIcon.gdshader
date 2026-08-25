shader_type canvas_item;

varying vec3 vertex_color;
void vertex() {
  vertex_color = COLOR.rgb;
}

void fragment() {
	vec4 tex = texture(TEXTURE, UV);
	vec3 outline_color = mix(vertex_color, vec3(0.66,0.66,0.66), 0.5);
	COLOR = vec4(
		mix(outline_color, vertex_color, tex.r),
		tex.a
	);
}
