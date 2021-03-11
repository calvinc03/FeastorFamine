// internal
#include "render.hpp"
#include "render_components.hpp"
#include "camera.hpp"
#include "ui.hpp"
#include "particle.hpp"

#include "text.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
#include <iostream>

#include "mob.hpp"
void RenderSystem::animate(entt::entity entity)
{

	auto &animate = registry.get<Animate>(entity);

	auto &sprite = *registry.get<ShadedMeshRef>(entity).reference_to_cache;

	float state_num = animate.state_num;
	float frame_num = animate.frame_num;

	float curr_state = animate.state;
	float curr_frame = animate.frame;

	vec2 scale_pos = {1.f, 1.f};
	vec2 scale_tex = {1.f, 1.f};

	// vec2 offset_pos = { 0.f, 0.f };
	vec2 offset_tex = {0.f, 0.f};

	scale_tex = {1 / frame_num, 1 / state_num};
	scale_pos = {1 / frame_num, 1 / state_num};
	offset_tex = {curr_frame / frame_num, curr_state / state_num};
	// offset_pos = { 0 / frame_num, 0 / state_num };

	// The position corresponds to the center of the texture.
	TexturedVertex vertices[4];

	vertices[0].position = {-1.f / 2 * scale_pos.x, +1.f / 2 * scale_pos.y, 0.f};
	vertices[1].position = {+1.f / 2 * scale_pos.x, +1.f / 2 * scale_pos.y, 0.f};
	vertices[2].position = {+1.f / 2 * scale_pos.x, -1.f / 2 * scale_pos.y, 0.f};
	vertices[3].position = {-1.f / 2 * scale_pos.x, -1.f / 2 * scale_pos.y, 0.f};
	vertices[0].texcoord = {0.f * scale_tex.x + offset_tex.x, 1.f * scale_tex.y + offset_tex.y};
	vertices[1].texcoord = {1.f * scale_tex.x + offset_tex.x, 1.f * scale_tex.y + offset_tex.y};
	vertices[2].texcoord = {1.f * scale_tex.x + offset_tex.x, 0.f * scale_tex.y + offset_tex.y};
	vertices[3].texcoord = {0.f * scale_tex.x + offset_tex.x, 0.f * scale_tex.y + offset_tex.y};

	// Counterclockwise as it's the default opengl front winding direction.
	uint16_t indices[] = {0, 3, 1, 1, 3, 2};

	// Vertex Buffer creation
	glBindBuffer(GL_ARRAY_BUFFER, sprite.mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // sizeof(TexturedVertex) * 4
	gl_has_errors();

	// Index Buffer creation
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite.mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // sizeof(uint16_t) * 6
	gl_has_errors();

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

void RenderSystem::drawTexturedMesh(entt::entity entity, const mat3 &projection)
{
	vec2 position = vec2();
	vec2 scale = vec2(1.0, 1.0);
	float angle = 0.f;

	if (registry.has<Motion>(entity))
	{
		auto &motion = registry.get<Motion>(entity);
		position = motion.position;
		scale = motion.scale;
		angle = motion.angle;
	}
	else if (registry.has<UI_element>(entity))
	{
		auto &ui_element = registry.get<UI_element>(entity);
		position = ui_element.position;
		scale = ui_element.scale;
	}

	// camera zoom
	auto view = registry.view<Motion>();
	auto &camera_scale = view.get<Motion>(camera).scale;
	//std::cout << camera_scale.x << ", " << camera_scale.y << "\n";
	auto &texmesh = *registry.get<ShadedMeshRef>(entity).reference_to_cache;

	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	Transform transform;
	if (registry.has<UI_element>(entity))
	{
		transform.translate(position);
		transform.rotate(angle);
		transform.scale(scale);
	}
	else
	{
		transform.translate(vec2({position.x * camera_scale.x, position.y * camera_scale.y}));
		transform.rotate(angle);
		transform.scale(vec2({scale.x * camera_scale.x, scale.y * camera_scale.y}));
	}

	// Setting shaders
	glUseProgram(texmesh.effect.program);
	glBindVertexArray(texmesh.mesh.vao);
	gl_has_errors();

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();

	GLint transform_uloc = glGetUniformLocation(texmesh.effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(texmesh.effect.program, "projection");
	gl_has_errors();

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, texmesh.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texmesh.mesh.ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(texmesh.effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(texmesh.effect.program, "in_texcoord");
	GLint in_color_loc = glGetAttribLocation(texmesh.effect.program, "in_color");
	if (in_texcoord_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void *>(0));
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void *>(sizeof(vec3))); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texmesh.texture.texture_id);
	}
	else if (in_color_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void *>(0));
		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void *>(sizeof(vec3)));
	}
	else
	{
		throw std::runtime_error("This type of entity is not yet supported");
	}

	//HIGHLIGHT for ui elements
	if (registry.has<HighlightBool>(entity))
	{
		GLint highlight_uloc = glGetUniformLocation(texmesh.effect.program, "highlight");
		if (highlight_uloc >= 0)
		{

			if (registry.get<HighlightBool>(entity).highlight)
			{ //check bool
				glUniform1i(highlight_uloc, 1);
			}
			else
			{
				glUniform1i(highlight_uloc, 0);
			}
		}
	}

	if (registry.has<HitReaction>(entity))
	{
		GLint hit_bool_uloc = glGetUniformLocation(texmesh.effect.program, "hit_bool");
		auto &hit_reaction = registry.get<HitReaction>(entity);
		if (hit_reaction.hit_bool)
		{
			glUniform1i(hit_bool_uloc, 1);
			hit_reaction.hit_bool = false;
		}
		else
		{
			glUniform1i(hit_bool_uloc, 0);
		}
	}

	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(texmesh.effect.program, "fcolor");
	glUniform3fv(color_uloc, 1, (float *)&texmesh.texture.color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();
	GLsizei num_indices = size / sizeof(uint16_t);
	//GLsizei num_triangles = num_indices / 3;

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.mat);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
}

// Draw the intermediate texture to the screen, with some distortion to simulate water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	glUseProgram(screen_sprite.effect.program);
	glBindVertexArray(screen_sprite.mesh.vao);
	gl_has_errors();

	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(&window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();

	// Disable alpha channel for mapping the screen texture onto the real screen
	glDisable(GL_BLEND); // we have a single texture without transparency. Areas with alpha <1 cab arise around the texture transparency boundary, enabling blending would make them visible.
	glDisable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, screen_sprite.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_sprite.mesh.ibo); // Note, GL_ELEMENT_ARRAY_BUFFER associates indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	// Draw the screen texture on the quad geometry
	gl_has_errors();

	// Set clock
	GLuint time_uloc = glGetUniformLocation(screen_sprite.effect.program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(screen_sprite.effect.program, "darken_screen_factor");
	glUniform1f(time_uloc, static_cast<float>(glfwGetTime() * 10.0f));

	auto view = registry.view<ScreenState>();
	auto &screen = view.get<ScreenState>(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();

	// Set the vertex position and vertex texture coordinates (both stored in the same VBO)
	GLint in_position_loc = glGetAttribLocation(screen_sprite.effect.program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	GLint in_texcoord_loc = glGetAttribLocation(screen_sprite.effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3)); // note the stride to skip the preceeding vertex position
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_sprite.texture.texture_id);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr); // two triangles = 6 vertices; nullptr indicates that there is no offset from the bound index buffer
	glBindVertexArray(0);
	gl_has_errors();
}

void RenderSystem::drawParticle(GLuint billboard_vertex_buffer, GLuint particles_position_buffer){
    
    // Update the buffers that OpenGL uses for rendering.
    // There are much more sophisticated means to stream data from the CPU to the GPU,
    // but this is outside the scope of this tutorial.
    // http://www.opengl.org/wiki/Buffer_Object_Streaming
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(GLfloat) * 2, g_particule_position_size_data);
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use our shader
    auto particle = registry.view<ParticleSystem>();
    auto& texmesh = *registry.get<ShadedMeshRef>(particle.front()).reference_to_cache;
    glUseProgram(texmesh.effect.program);
    glBindVertexArray(texmesh.mesh.vao);
    gl_has_errors();
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texmesh.texture.texture_id);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
//    glUniform1i(TextureID, 0);

    // Same as the billboards tutorial
//    glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
//    glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
//
//    glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0
    );
        
    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0
    );

    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
//    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 2, PARTICLE_COUNT);
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	ivec2 frame_buffer_size; // in pixels
	glfwGetFramebufferSize(&window, &frame_buffer_size.x, &frame_buffer_size.y);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// Clearing backbuffer
	glViewport(0, 0, frame_buffer_size.x, frame_buffer_size.y);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 1, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();

	auto view = registry.view<Motion>();
	auto &camera_motion = view.get<Motion>(camera);
	//std::cout << camera_motion.position.x << ", " << camera_motion.position.y << " | " << camera_motion.velocity.x << ", " << camera_motion.velocity.y << "\n";
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f + camera_motion.position.x;
	float top = 0.f + camera_motion.position.y;
	float right = WINDOW_SIZE_IN_PX.x + camera_motion.position.x;
	float bottom = WINDOW_SIZE_IN_PX.y + camera_motion.position.y;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};

	// some repeated code for the ui matrix -- any suggestions on how to avoid this?
	float left_ui = 0.f;
	float top_ui = 0.f;
	float right_ui = WINDOW_SIZE_IN_PX.x;
	float bottom_ui = WINDOW_SIZE_IN_PX.y;

	float sx_ui = 2.f / (right_ui - left_ui);
	float sy_ui = 2.f / (top_ui - bottom_ui);
	float tx_ui = -(right_ui + left_ui) / (right_ui - left_ui);
	float ty_ui = -(top_ui + bottom_ui) / (top_ui - bottom_ui);
	mat3 projection_2D_ui{{sx_ui, 0.f, 0.f}, {0.f, sy_ui, 0.f}, {tx_ui, ty_ui, 1.f}};

	auto view_mesh_ref_motion = registry.view<ShadedMeshRef, Motion>();
	auto view_mesh_ref_ui = registry.view<UI_element, ShadedMeshRef>();

	auto view_nodes = registry.view<GridNode>();

	auto view_mesh_ref = registry.view<ShadedMeshRef>();

    static const GLfloat g_vertex_buffer_data[] = {
         -0.5f, -0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
         -0.5f,  0.5f, 0.0f,
          0.5f,  0.5f, 0.0f,
    };
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);

    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 2 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	
	std::vector<std::vector<entt::entity>> sort_by_layer = {};

	// 100 layers
	sort_by_layer.resize(100);
    
	for (entt::entity entity : view_mesh_ref)
	{
		int layer = view_mesh_ref.get<ShadedMeshRef>(entity).layer;
		sort_by_layer[layer].push_back(entity);

	}
	for (auto entities : sort_by_layer)
	{
		for (auto entity : entities)
		{
			if (view_mesh_ref.get<ShadedMeshRef>(entity).show)
			{
				if (registry.has<Animate>(entity))
				{
					animate(entity);
				}
				if (registry.has<UI_element>(entity))
				{
					drawTexturedMesh(entity, projection_2D_ui);
				}
                else if (registry.has<ParticleSystem>(entity))
                {
                    continue;
                }
				else
				{
					drawTexturedMesh(entity, projection_2D);
				}
				if (registry.has<Text>(entity)) {

					drawText(registry.get<Text>(entity), frame_buffer_size);
				}
				gl_has_errors();
			}

		}
	}

	//useful for rendering entities with only text and no ShadedMeshRef
	auto view_text = registry.view<Text>();
	for (auto [entity, text] : view_text.each()) 	{
		if(!registry.has<ShadedMeshRef>(entity))
			drawText(text, frame_buffer_size);
	}
    
    // Truely render to the screen
    if (registry.view<ParticleSystem>().size() != 0) {
        std::cout << "Draw particle \n";
        drawParticle(billboard_vertex_buffer, particles_position_buffer);
    }

	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(&window);
}

void gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR)
		return;

	const char *error_str = "";
	while (error != GL_NO_ERROR)
	{
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		std::cerr << "OpenGL:" << error_str << std::endl;
		error = glGetError();
	}
	throw std::runtime_error("last OpenGL error:" + std::string(error_str));
}
