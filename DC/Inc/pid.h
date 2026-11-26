#ifndef PID_H
#define PID_H

/*
 * Bộ điều khiển PID rời rạc (không phụ thuộc HAL/RTOS).
 *  - Anti-windup: chỉ cộng dồn I khi đầu ra chưa bão hòa (hoặc khi sai số kéo đầu ra quay lại).
 *  - D tính theo giá trị đo (không theo sai số) -> không bị "derivative kick" khi đổi tốc độ đặt.
 *  - D có bộ lọc thông thấp bậc 1.
 *
 * Cách dùng (gọi đều đặn mỗi dt giây, ví dụ 20 ms):
 *     PID_Typedef pid;
 *     PID_Init(&pid, 0.8f, 2.0f, 0.0f, 0.0f, 100.0f, 0.8f);   // Kp, Ki, Kd, min, max, alpha
 *     ...
 *     float duty = PID_Update(&pid, rpm_set, rpm_meas, 0.02f); // duty trong 0..100 (%)
 */

typedef struct {
    float kp, ki, kd;
    float out_min, out_max;   // giới hạn đầu ra (ví dụ 0..100 % PWM)
    float integral;           // thành phần I đã cộng dồn (đã nhân Ki)
    float prev_meas;          // giá trị đo ở lần trước
    float d_filt;             // thành phần đạo hàm đã lọc
    float alpha;              // 0: không lọc, gần 1: lọc mạnh
    int   first;              // cờ lần chạy đầu
} PID_Typedef;

static inline void PID_Reset(PID_Typedef *p) {
    p->integral  = 0.0f;
    p->prev_meas = 0.0f;
    p->d_filt    = 0.0f;
    p->first     = 1;
}

static inline void PID_Init(PID_Typedef *p, float kp, float ki, float kd,
                            float out_min, float out_max, float alpha) {
    p->kp = kp;
    p->ki = ki;
    p->kd = kd;
    p->out_min = out_min;
    p->out_max = out_max;
    p->alpha = alpha;
    PID_Reset(p);
}

static inline float PID_Update(PID_Typedef *p, float setpoint, float meas, float dt) {
    float err = setpoint - meas;

    if (p->first) {            // tránh nhảy D ở lần đầu
        p->prev_meas = meas;
        p->first = 0;
    }

    // D theo giá trị đo (dấu âm vì đạo hàm của sai số = -đạo hàm của giá trị đo)
    float d_raw = -(meas - p->prev_meas) / dt;
    p->prev_meas = meas;
    p->d_filt = p->alpha * p->d_filt + (1.0f - p->alpha) * d_raw;

    float i_new = p->integral + p->ki * err * dt;
    float out   = p->kp * err + i_new + p->kd * p->d_filt;

    // Anti-windup: khi bão hòa chỉ cho I thay đổi nếu sai số kéo đầu ra quay lại
    if (out > p->out_max) {
        out = p->out_max;
        if (err < 0) p->integral = i_new;
    } else if (out < p->out_min) {
        out = p->out_min;
        if (err > 0) p->integral = i_new;
    } else {
        p->integral = i_new;
    }
    return out;
}

#endif /* PID_H */
