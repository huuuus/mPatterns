#pragma once 

#define GET_R(c) ((c&0xFF000000)>>24) 
#define GET_G(c) ((c&0x00FF0000)>>16) 
#define GET_B(c) ((c&0x0000FF00)>>8) 

inline float fastFitness_RGBA(u32 s, u32 r) {
    s32 lumS = (GET_R(s)+GET_G(s)+GET_B(s))/3;
    s32 lumR = (GET_R(r)+GET_G(r)+GET_B(r))/3;
    float diff = (float)(lumS-lumR);
            
    // more sample than ref, this is bad
    if (diff>0)
        return diff/64.f;
    else
        return -diff;
}
        
//        float fitness(float sample, float ref) {
//            float diff = sample-ref;
//            if (diff>0) {
//                // more sample than ref
//                diff = diff/100.f;
//            }
//            else {
//                // more ref than sample
//                diff = abs(diff);
//            }  
//            return diff;
//        }
//        
//        float d_lum(Colorf sample, Colorf ref) {
//            Vec3f ref_HSV = ref.get( CM_HSV );    
//            Vec3f sample_HSV = sample.get( CM_HSV );
//            return fitness(sample_HSV[2], ref_HSV[2]);
//        }
// 
//        float d_tone(Colorf sample, Colorf ref) {
//            Vec3f ref_HSV = ref.get( CM_HSV );    
//            Vec3f sample_HSV = sample.get( CM_HSV );
//            return abs(ref_HSV[1]-sample_HSV[1]) + abs(ref_HSV[0]-sample_HSV[0]);
//        }
