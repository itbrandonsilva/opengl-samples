class Square {

public:

	GLfloat vertices[12];
	GLuint vbID;

	Square(int w, int h) {

		GLfloat v[] = {
			0, 0,
			0, h,
			w, 0,

			w, 0,
			0, h,
			w, h
		};

		for (unsigned int i = 0; i < 12; ++i) vertices[i] = v[i];

		glGenBuffers(1, &vbID);
		glBindBuffer(GL_ARRAY_BUFFER, vbID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}

	~Square(){}

	void Draw(int x, int y, glm::vec4 color) {

		GLuint ModelUniform = glGetUniformLocation(ShaderId, "model");
		GLuint ProjectionUniform = glGetUniformLocation(ShaderId, "projection");
		GLuint VertexColorUniform = glGetUniformLocation(ShaderId, "vertexColor");

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
		glUniformMatrix4fv(ModelUniform, 1, GL_FALSE, &model[0][0]);

		glm::mat4 projection = glm::mat4( glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, 0.0f, 100.0f) );
		// Whats wrong with this?
		// glm::mat4 projection( glm::ortho(0, width, height, 0, 0, 100) );
		glUniformMatrix4fv(ProjectionUniform, 1, GL_FALSE, &projection[0][0]);

		glUniform3f(VertexColorUniform, color.x, color.y, color.z);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbID);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // each vertex consists of 2 floats
			GL_FLOAT,           // each value is a float
			GL_FALSE,           // normalized?
			0,                  // stride; dont skip over any data
			(void*)0            // start at the first element in the array
		);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

		glDisableVertexAttribArray(0);
	}
};