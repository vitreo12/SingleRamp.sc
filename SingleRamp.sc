SingleRamp : UGen {
    *ar { arg trig, freq = 1;
        ^this.multiNew('audio', trig, freq)
    }
}