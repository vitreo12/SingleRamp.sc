SingleRamp : UGen {
    *ar { arg trig, freq = 1, sync = 1;
        ^this.multiNew('audio', trig, freq, sync)
    }
}