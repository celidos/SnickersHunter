#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>


GLfloat* getSphere(int vSteps, int hSteps, GLfloat R, int &bufSize)
{
    int total_triangles = 2 * (vSteps - 2) * hSteps; // + 1 * 2 * circleSteps;
    int total_buffer_size = 3 * 3 * total_triangles;

    bufSize = total_buffer_size;

    GLfloat* buffer = new GLfloat[total_buffer_size];

    int b_ind = 0;
    for (int theta_frac = 1; theta_frac < vSteps - 1; ++theta_frac) {
        for (int alpha_frac = 0; alpha_frac < hSteps; ++alpha_frac) {
            GLfloat theta_0 = M_PI * theta_frac / vSteps;
            GLfloat theta_1 = M_PI * (theta_frac + 1) / vSteps;

            GLfloat alpha_0 = 2 * M_PI * alpha_frac / hSteps;
            GLfloat alpha_1 = 2 * M_PI * (alpha_frac + 1) / hSteps;

            GLfloat point0[] = {
                R * sin(theta_0) * sin(alpha_0),
                R * cos(theta_0),
                R * sin(theta_0) * cos(alpha_0)
            };
            GLfloat point1[] = {
                R * sin(theta_0) * sin(alpha_1),
                R * cos(theta_0),
                R * sin(theta_0) * cos(alpha_1)
            };
            GLfloat point2[] = {
                R * sin(theta_1) * sin(alpha_0),
                R * cos(theta_1),
                R * sin(theta_1) * cos(alpha_0)
            };
            GLfloat point3[] = {
                R * sin(theta_1) * sin(alpha_1),
                R * cos(theta_1),
                R * sin(theta_1) * cos(alpha_1)
            };
            buffer[b_ind  ] = point0[0];
            buffer[b_ind+1] = point0[1];
            buffer[b_ind+2] = point0[2];

            buffer[b_ind+3] = point1[0];
            buffer[b_ind+4] = point1[1];
            buffer[b_ind+5] = point1[2];

            buffer[b_ind+6] = point2[0];
            buffer[b_ind+7] = point2[1];
            buffer[b_ind+8] = point2[2];


            buffer[b_ind+9] = point2[0];
            buffer[b_ind+10] = point2[1];
            buffer[b_ind+11] = point2[2];

            buffer[b_ind+12] = point3[0];
            buffer[b_ind+13] = point3[1];
            buffer[b_ind+14] = point3[2];

            buffer[b_ind+15] = point1[0];
            buffer[b_ind+16] = point1[1];
            buffer[b_ind+17] = point1[2];

            b_ind += 2 * 3 * 3;
        }
    }

    return buffer;
}
