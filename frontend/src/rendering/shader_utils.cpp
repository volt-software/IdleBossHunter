/*
    IdleBossHunter client
    Copyright (C) 2020  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include "spdlog/spdlog.h"

#include "shader_utils.h"

using namespace std;

void print_shader_log(GLuint const shader) noexcept {
    if(glIsShader(shader)) {
        int infoLogLength = 0;
        int maxLength = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        char* infoLog = new char[maxLength];

        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0) {
            spdlog::error("[shader_utils] infolog {}", infoLog);
        }

        delete[] infoLog;
    } else {
        spdlog::error("[shader_utils] Name {} is not a shader", shader);
    }
}

void print_program_log(GLuint const program) noexcept {
    if(glIsProgram(program)) {
        int infoLogLength = 0;
        int maxLength = 0;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        char* infoLog = new char[maxLength];

        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0) {
            spdlog::error("[shader_utils] infolog {}", infoLog);
        }

        delete[] infoLog;
    } else {
        spdlog::error("[shader_utils] Name {} is not a program", program);
    }
}

optional<GLuint> const load_shader_from_file(string const & path, GLenum const shaderType) noexcept {
    //Open file
    GLuint shaderID = 0;
    string shaderString;
    ifstream sourceFile(path);

    //Source file loaded
    if(!sourceFile) {
        spdlog::error("[shader_utils] Unable to open shader file {}", path);
        return {};
    }

    //Get shader source
    shaderString.assign((istreambuf_iterator<char>(sourceFile)), istreambuf_iterator<char>());

    shaderID = glCreateShader(shaderType);
    spdlog::info("[shader_utils] loading & compiling shader {} with path {}", shaderID, path);

    const GLchar* shaderSource = shaderString.c_str();
    glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);

    glCompileShader(shaderID);

    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
    if(shaderCompiled != GL_TRUE) {
        spdlog::error("[shader_utils] Unable to compile shader {}", shaderID);
        spdlog::error("[shader_utils] Source {}", shaderSource);
        print_shader_log(shaderID);
        glDeleteShader(shaderID);
        return {};
    }

    return make_optional(shaderID);
}
