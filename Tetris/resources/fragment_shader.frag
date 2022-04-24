#version 330 core
out vec4 FragColor;
uniform sampler2D tex1;
uniform vec4 color;
uniform float tetris_board_top_boundary;
uniform int cut_off;
uniform int is_block;
uniform int is_ghost;
uniform int inverse;
uniform int textured;
in vec4 pos;
in vec2 uv;

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	if (cut_off == 1 && gl_FragCoord.y >= tetris_board_top_boundary)
	{
		discard;
	}

	vec4 final_color;
	if (textured == 1)
	{
		//vec4 texture_color = vec4(uv.x, uv.y, 0, 1);
		//texture_color.rgb *= texture(tex1, vec2(uv.x, uv.y)).rgb;
		vec4 texture_color = texture(tex1, vec2(uv.x, uv.y));
		final_color = color * texture_color;
	}
	else
	{
		final_color = color;
	}

	if (is_block == 1)
	{
		if (is_ghost == 1)
		{
			float cell_size = 32.0;
			float c = 3.0/cell_size;
			float c2 = 2.0/cell_size;
			if ((pos.x < c2 && pos.y <= c2) ||
				(pos.x >= 1-c2 && pos.y <= c2) ||
				(pos.x < c2 && pos.y > 1-c2) ||
				(pos.x >= 1-c2 && pos.y > 1-c2))
			{
				discard;
			}

			if ((pos.x < 1-c && pos.y <= 1-c) &&
				(pos.x >= c && pos.y > c))
			{
				final_color = vec4(color.rgb * 1.25, 0.1);
			}
			else
			{
				final_color = vec4(color.rgb * 1.25, 0.5);
			}
		}
		else
		{
			float cell_size = 32.0;
			float c = 3.0/cell_size;
			float c2 = 1.0/cell_size;
			if ((pos.x < c2 && pos.y <= c2) ||
				(pos.x >= 1-c2 && pos.y <= c2) ||
				(pos.x < c2 && pos.y > 1-c2) ||
				(pos.x >= 1-c2 && pos.y > 1-c2))
			{
				discard;
			}

			if ((pos.x < 1-c && pos.y <= 1-c) &&
				(pos.x >= c && pos.y > c))
			{
				final_color = vec4(color.rgb * 0.8, 1.0);
			}
			else
			{
				final_color = vec4(color.rgb, 1.0);
			}
		}
	}
	
	if (is_block == 1 && inverse == 1)
	{
			final_color.rgb = step(vec3(1, 1, 1), final_color.rgb);
			final_color.r = final_color.r + final_color.g + final_color.b;
			final_color.g = final_color.r + final_color.g + final_color.b;
			final_color.b = final_color.r + final_color.g + final_color.b;
	}

	FragColor = final_color;
}
