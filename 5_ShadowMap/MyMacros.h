#pragma once

#define MY_TITLE "OpenGLRender"

// some mode variable

#define MY_MODE_SHADOW "shadowmode"
#define MY_MODE_FILTER "filtermode"

// shader filename
#define MY_SHADER_DEPTHMAP_VERT "genDepthmap.vert"
#define MY_SHADER_DEPTHMAP_FRAG "genDepthmap.frag"

#define MY_SHADER_MAIN_VERT "3.3.shader.vert"
#define MY_SHADER_MAIN_FRAG "3.3.shader.frag"

#define  MY_SHADER_LIGHT_VERT "lightShader.vert"
#define  MY_SHADER_LIGHT_FRAG "lightShader.frag"

// shader uniform struct variable
#define MY_POINTLIGHT_AMBIENT "light.ambient"
#define MY_POINTLIGHT_DIFFUSE "light.diffuse"
#define MY_POINTLIGHT_SPECULAR "light.specular"
#define MY_POINTLIGHT_POS "light.Pos"
#define MY_POINTLIGHT_COLOR "light.Color"

#define MY_MATERIAL_SHININESS "material.shininess"
#define MY_MATERIAL_USE_SPECULARMAP "material.useSpecularMap"

#define MY_CAMERA_POS "cameraPos"


// MATRIX
#define MY_MATRIX_MODEL "model"
#define MY_MATRIX_VIEW "view"
#define MY_MATRIX_PROJ "projection"
#define MY_MATRIX_LIGHTSPACE "lightSpaceMatrix"
