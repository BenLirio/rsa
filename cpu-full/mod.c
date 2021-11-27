
void Mod(bigInt dst, bigInt src, bigInt m) {
    
}

/*
uint64 uint64_clz(uint64 a) {
    asm("clz x0, x0");
}

int Clz(bigInt a) {
    int s = 0;
    for (int i = 2*(2048>>6)-1; i >= 0; i--) {
        if (a[i] == 0) {
            s += 1<<6;
        } else {
            s += uint64_clz(a[i]);
            break;
        }
    }
    return s;
}


int Cmp(bigInt a, bigInt b) {
    for (int i = 2*(2048>>6)-1; i >= 0; i--) {
        if (a[i] == b[i]) {
            continue;
        }
        return a[i] > b[i] ? 1 : -1;
    }
    return 0;
}

void Sub(bigInt dst, bigInt a, bigInt b) {
    bool c;
    bool prev = false;
    for (int i = 0; i < 2*(2048>>6); i++) {
        if (a[i] < b[i]+prev) {
            c = true;
        } else {
            c = false;
        }
        dst[i] = a[i] - b[i] - prev;
        prev = c;
    }
}

void LeftShift(bigInt a) {
    bool c;
    bool prev = false;
    for (int i = 0; i < 2*(2048>>6); i++) {
        c = a[i]&((uint64)1<<63);
        a[i] <<= 1;
        a[i] |= prev;
        prev = c;
    }
}

void RightShift(bigInt a) {
    bool c;
    bool prev = false;
    for (int i = 2*(2048>>6); i >= 0; i--) {
        c = a[i]&1;
        a[i] >>= 1;
        a[i] |= ((uint64)prev)<<63;
        prev = c;
    }
}

void Mod(bigInt dst, bigInt src, bigInt m) {
    bigInt tmp = calloc(1, 2*(2048>>3));
    for (int i = 0; i < 2*(2048>>6); i++) {
        tmp[i] = src[i];
    }
    int s = Clz(m);
    for (int i = 0; i < s; i++) {
        LeftShift(m);
    }
    for (int i = 0; i <= s; i++) {
        if (Cmp(tmp, m) == 1) {
            Sub(tmp, tmp, m);
        }
        RightShift(m);
    }
    for (int i = 0; i < 2*(2048>>6); i++) {
        dst[i] = tmp[i];
    }
    free(tmp);
}
*/
