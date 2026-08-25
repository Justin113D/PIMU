shader_type canvas_item;

uniform sampler2D mask_texture;
uniform sampler2D active_texture : filter_linear_mipmap;

varying vec4 vertex_color;
void vertex() {
  vertex_color = COLOR;
}

void fragment() {
	COLOR = mix(
		texture(TEXTURE, UV), 
		texture(active_texture, UV), 
		texture(mask_texture, UV).r
	) * vertex_color;
}