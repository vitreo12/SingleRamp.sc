#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct SingleRamp : public Unit 
{
    double phase = 0.0;
    double freq_incr = 0.0;
};

static void SingleRamp_next_a_a(SingleRamp* unit, int inNumSamples);
static void SingleRamp_next_a_k(SingleRamp* unit, int inNumSamples);
//static void SingleRamp_next_k_a(SingleRamp* unit, int inNumSamples);
//static void SingleRamp_next_k_k(SingleRamp* unit, int inNumSamples);

static void SingleRamp_Ctor(SingleRamp* unit);

void SingleRamp_Ctor(SingleRamp* unit) 
{
    /*
    if(INRATE(0) == calc_FullRate) 
    {
        if(INRATE(1) == calc_FullRate)
        {
            //Print("a_a\n");
            SETCALC(SingleRamp_next_a_a);
        }
        else
        {
            //Print("a_k\n");
            SETCALC(SingleRamp_next_a_k);
        } 
    } 
    else 
    {
        if(INRATE(1) == calc_FullRate)
        {
            //Print("k_a\n");
            SETCALC(SingleRamp_next_k_a);
        }
        else
        {
            //Print("k_k\n");
            SETCALC(SingleRamp_next_k_k);
        }
    }
    */

    if(INRATE(1) == calc_FullRate)
    {
        Print("a_a\n");
        SETCALC(SingleRamp_next_a_a);
    }
    else
    {
        Print("a_k\n");
        SETCALC(SingleRamp_next_a_k);
    } 
    
    //calc one sample
    SingleRamp_next_a_k(unit, 1);
}

void SingleRamp_next_a_a(SingleRamp* unit, int inNumSamples) 
{
    float* trig = IN(0);
    float* freq = IN(1);

    int sync = (int)IN0(2);

    float* out = OUT(0);

    double out_val = unit->phase;

    double freq_incr;

    switch (sync)
    {
        //Reset frequency only on trigger reset
        case 1:
            for(int i = 0; i < inNumSamples; i++)
            {
                float trig_val = trig[i];

                //Reset
                if(trig_val > 0)
                {
                    float freq_val = freq[i];

                    //negative freq, convert to positive
                    if(freq_val < 0)
                        freq_val = -freq_val;

                    unit->freq_incr = freq_val / SAMPLERATE;

                    out_val = 0.0;
                }

                freq_incr = unit->freq_incr;

                out_val += freq_incr;

                //Limit at 1 and wait for reset
                if(out_val >= 1.0)
                    out_val = 1.0;
                
                out[i] = out_val;
            }

            break;
        
        //Update frequency constantly (at ar rate)
        default:
            for(int i = 0; i < inNumSamples; i++)
            {
                float trig_val = trig[i];
                float freq_val = freq[i];

                //negative freq, convert to positive
                if(freq_val < 0)
                    freq_val = -freq_val;
                
                freq_incr = freq_val / SAMPLERATE;

                //Reset
                if(trig_val > 0)
                    out_val = 0.0;

                out_val += freq_incr;

                //Limit at 1 and wait for reset
                if(out_val >= 1.0)
                    out_val = 1.0;
                
                out[i] = out_val;
            }
            
            break;
    }

    unit->phase = out_val;
}

void SingleRamp_next_a_k(SingleRamp* unit, int inNumSamples) 
{
    float* trig    = IN(0);
    float freq_val = IN0(1);

    int sync = (int)IN0(2);

    float* out = OUT(0);

    double out_val = unit->phase;

    double freq_incr;

    switch (sync)
    {
        //Reset frequency only on trigger reset
        case 1:
            for(int i = 0; i < inNumSamples; i++)
            {
                float trig_val = trig[i];

                //Reset
                if(trig_val > 0)
                {   
                    //negative freq, convert to positive
                    if(freq_val < 0)
                        freq_val = -freq_val;

                    unit->freq_incr = freq_val / SAMPLERATE;

                    out_val = 0.0;
                }

                freq_incr = unit->freq_incr;

                out_val += freq_incr;

                //Limit at 1 and wait for reset
                if(out_val >= 1.0)
                    out_val = 1.0;
                
                out[i] = out_val;        
            }

            break;
        
        //Update frequency always (at kr rate)
        default:
            //negative freq, convert to positive
            if(freq_val < 0)
                freq_val = -freq_val;

            freq_incr = freq_val / SAMPLERATE;

            for(int i = 0; i < inNumSamples; i++)
            {
                float trig_val = trig[i];

                //Reset
                if(trig_val > 0)
                    out_val = 0.0;

                out_val += freq_incr;

                //Limit at 1 and wait for reset
                if(out_val >= 1.0)
                    out_val = 1.0;
                
                out[i] = out_val;        
            }

            break;
    }

    unit->phase = out_val;
}

/*
void SingleRamp_next_k_a(SingleRamp* unit, int inNumSamples) 
{
    float trig_val = IN0(0);
    float* freq    = IN(1);

    float* out = OUT(0);

    double out_val = unit->phase;

    //Reset
    if(trig_val > 0)
        out_val = 0.0;

    for(int i = 0; i < inNumSamples; i++)
    {
        float freq_val = freq[i];

        //negative freq, convert to positive
        if(freq_val < 0)
            freq_val = -freq_val;

        double freq_incr = freq_val / SAMPLERATE;
        
        out_val += freq_incr;

        //Limit at 1 and wait for reset
        if(out_val >= 1.0)
            out_val = 1.0;
        
        out[i] = out_val;
    }

    unit->phase = out_val;
}

void SingleRamp_next_k_k(SingleRamp* unit, int inNumSamples) 
{
    float trig_val = IN0(0);
    float freq_val = IN0(1);

    //negative freq, convert to positive
    if(freq_val < 0)
        freq_val = -freq_val;

    float* out = OUT(0);

    double freq_incr = freq_val / SAMPLERATE;

    double out_val = unit->phase;

    //Reset
    if(trig_val > 0)
        out_val = 0.0;

    for(int i = 0; i < inNumSamples; i++)
    {
        out_val += freq_incr;

        //Limit at 1 and wait for reset
        if(out_val >= 1.0)
            out_val = 1.0;
        
        out[i] = out_val;
    }

    unit->phase = out_val;
}
*/

PluginLoad(SingleRampUGens) 
{
    ft = inTable; 
    DefineSimpleUnit(SingleRamp);
}