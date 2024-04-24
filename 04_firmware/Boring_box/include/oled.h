#ifndef __OLED_H
#define __OLED_H

#define DRAW_EMOJI_CASE(NUM)     \
    case NUM:                    \
        drawbitmap(EMOJI_##NUM); \
        break;

#include "emoji.h"
#include <Wire.h>
#include "debug.h"
#include <Arduino.h>
#include "gif.h"

#define Play_gif_times 2

class OLED
{
public:
    struct GIF
    {
        const uint8_t **frames; // 指向帧数组的指针
        uint8_t frameCount;     // 帧数
    };
    void init();
    void random_emoji();
    void displayAnimation(const uint8_t *bitmaps[], unsigned int count, unsigned int delayTime);
    void power_on_gif();
    void random_gif();

private:
    const uint8_t *bitmaps[22] = {LOGO_A, LOGO_B, LOGO_C, LOGO_D, LOGO_E, LOGO_F, LOGO_G, LOGO_H, LOGO_I, LOGO_J, LOGO_K, LOGO_L, LOGO_M, LOGO_N, LOGO_O, LOGO_P, LOGO_Q, LOGO_R, LOGO_S, LOGO_T, LOGO_U, LOGO_V};
    const uint8_t *emoji[29] = {EMOJI_1, EMOJI_2, EMOJI_3, EMOJI_4, EMOJI_5, EMOJI_6, EMOJI_7, EMOJI_8, EMOJI_9, EMOJI_10, EMOJI_11, EMOJI_12, EMOJI_13, EMOJI_14, EMOJI_15, EMOJI_16, EMOJI_17, EMOJI_18, EMOJI_19, EMOJI_20, EMOJI_21, EMOJI_22, EMOJI_23, EMOJI_24, EMOJI_25, EMOJI_26, EMOJI_27, EMOJI_28, EMOJI_29};
    const uint8_t *gif_A[4] = {GIF_A_1, GIF_A_2, GIF_A_3, GIF_A_4};
    const uint8_t *gif_B[4] = {GIF_B_1, GIF_B_2, GIF_B_3, GIF_B_4};
    const uint8_t *gif_C[4] = {GIF_C_1, GIF_C_2, GIF_C_3, GIF_C_4};
    const uint8_t *gif_D[4] = {GIF_D_1, GIF_D_2, GIF_D_3, GIF_D_4};
    const uint8_t *gif_E[4] = {GIF_E_1, GIF_E_2, GIF_E_3, GIF_E_4};
    const uint8_t *gif_F[4] = {GIF_F_1, GIF_F_2, GIF_F_3, GIF_F_4};
    const uint8_t *gif_G[4] = {GIF_G_1, GIF_G_2, GIF_G_3, GIF_G_4};
    const uint8_t *gif_H[4] = {GIF_H_1, GIF_H_2, GIF_H_3, GIF_H_4};
    const uint8_t *gif_I[4] = {GIF_I_1, GIF_I_2, GIF_I_3, GIF_I_4};
    const uint8_t *gif_J[4] = {GIF_J_1, GIF_J_2, GIF_J_3, GIF_J_4};
    const uint8_t *gif_K[4] = {GIF_K_1, GIF_K_2, GIF_K_3, GIF_K_4};
    const uint8_t *gif_L[4] = {GIF_L_1, GIF_L_2, GIF_L_3, GIF_L_4};
    const uint8_t *gif_M[4] = {GIF_M_1, GIF_M_2, GIF_M_3, GIF_M_4};
    const uint8_t *gif_N[4] = {GIF_N_1, GIF_N_2, GIF_N_3, GIF_N_4};
    const uint8_t *gif_O[4] = {GIF_O_1, GIF_O_2, GIF_O_3, GIF_O_4};
    const uint8_t *gif_P[4] = {GIF_P_1, GIF_P_2, GIF_P_3, GIF_P_4};
    const uint8_t *gif_Q[4] = {GIF_Q_1, GIF_Q_2, GIF_Q_3, GIF_Q_4};
    const uint8_t *gif_R[4] = {GIF_R_1, GIF_R_2, GIF_R_3, GIF_R_4};
    const uint8_t *gif_S[4] = {GIF_S_1, GIF_S_2, GIF_S_3, GIF_S_4};
    const uint8_t *gif_T[4] = {GIF_T_1, GIF_T_2, GIF_T_3, GIF_T_4};
    const uint8_t *gif_U[4] = {GIF_U_1, GIF_U_2, GIF_U_3, GIF_U_4};
    const uint8_t *gif_V[4] = {GIF_V_1, GIF_V_2, GIF_V_3, GIF_V_4};
    const uint8_t *gif_W[4] = {GIF_W_1, GIF_W_2, GIF_W_3, GIF_W_4};
    const uint8_t *gif_X[4] = {GIF_X_1, GIF_X_2, GIF_X_3, GIF_X_4};
    const uint8_t *gif_Y[4] = {GIF_Y_1, GIF_Y_2, GIF_Y_3, GIF_Y_4};
    const uint8_t *gif_Z[4] = {GIF_Z_1, GIF_Z_2, GIF_Z_3, GIF_Z_4};

    GIF gifs[26] = {
        {gif_A, 4}, {gif_B, 4}, {gif_C, 4}, {gif_D, 4}, {gif_E, 4}, {gif_F, 4}, {gif_G, 4}, {gif_H, 4}, {gif_I, 4}, {gif_J, 4}, {gif_K, 4}, {gif_L, 4}, {gif_M, 4}, {gif_N, 4}, {gif_O, 4}, {gif_P, 4}, {gif_Q, 4}, {gif_R, 4}, {gif_S, 4}, {gif_T, 4}, {gif_U, 4}, {gif_V, 4}, {gif_W, 4}, {gif_X, 4}, {gif_Y, 4}, {gif_Z, 4}};

    void drawbitmap(const uint8_t *bitmap);
};
extern OLED oled;

#endif
