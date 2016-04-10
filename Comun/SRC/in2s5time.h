#ifndef IN2S5TIME_H
#define IN2S5TIME_H

class In2S5Time
{
public:
    In2S5Time();
    int getInt(unsigned short val);    
    unsigned short getS5(int val);
    unsigned char getEscalaFromS5(unsigned short val);
    int escalaToInt(unsigned char escala);



};

#endif // IN2S5TIME_H
