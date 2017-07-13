#ifndef PROTOTYPES_H
#define PROTOTYPES_H

/* functions with optional arguments */
inline void move_up(uint8_t spd, uint8_t PID = 1);
inline void move_right(uint8_t spd, uint8_t PID = 1);
inline void move_back(uint8_t spd, uint8_t PID = 1);
inline void move_left(uint8_t spd, uint8_t PID = 1);
inline void move_up_right(uint8_t spd, uint8_t PID = 1);
inline void move_back_right(uint8_t spd, uint8_t PID = 1);
inline void move_back_left(uint8_t spd, uint8_t PID = 1);
inline void move_up_left(uint8_t spd, uint8_t PID = 1);

#endif
