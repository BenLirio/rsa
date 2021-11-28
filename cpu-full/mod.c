uint64 uint64_clz(uint64 a) {
    asm("clz x0, x0");
}

void LeftShift(bigInt a, int size) {
    bool c;
    bool prev = false;
    for (int i = 0; i < size; i++) {
        c = a[i]&((uint64)1<<63);
        a[i] <<= 1;
        a[i] |= prev;
        prev = c;
    }
}


int Clz(bigInt a, int size) {
    int s = 0;
    for (int i = size-1; i >= 0; i--) {
        if (a[i] == 0) {
            s += 1<<6;
        } else {
            s += uint64_clz(a[i]);
            break;
        }
    }
    return s;
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

void RemHelper(uint32* dst, uint32* u, uint32* v) {
    // u (2048>>5) + (32>>5)
    // v (2048>>5)
    uint64 mask32 = (((uint64)1)<<32) - 1;
    uint64 top0 = ((uint64)u[2048>>5]);
    uint64 top1 = u[(2048>>5)-1];
    uint64 bot = v[(2048>>5)-1];
    uint64 top =  top0<<32 + top1;
    uint64 q_hat = top/bot;
    uint64 r_hat =  top%bot;
    for (;;) {
        if (q_hat > mask32) {
            r_hat += bot*(q_hat-mask32);
            q_hat = mask32;
            printf("a");
            continue;
        }
        if (q_hat*v[(2048>>5)-2] > (r_hat<<32) + u[(2048>>5)-2]) {
            q_hat -= 1;
            r_hat += bot;
            printf("b");
            continue;
        }
        break;
    }
    uint32* tmp = malloc((2048>>3) + (32>>3));
    tmp[0] = (q_hat*v[0])&mask32;
    for (int i = 1; i < (2048>>5); i++) {
        tmp[i] = ((q_hat*v[i-1])>>32) + ((q_hat*v[i])&mask32);
    }
    tmp[(2048>>5)] = (q_hat*v[(2048>>5)-(32>>5)])>>32;
    bool c;
    bool prev = false;
    for (int i = 0; i < (2048>>5); i++) {
        if ((uint64)u[i] < ((uint64)tmp[i])+prev) {
            c = true;
        } else {
            c = false;
        }
        dst[i] = u[i] - tmp[i] - prev;
        prev = c;
    }
    if (u[(2048>>5)] > tmp[(2048>>5)]+prev) {
        printf("%d, %.8x, %.8x\n", prev, u[(2048>>5)], tmp[(2048>>5)]+prev);
    } else {
    }

    free(tmp);
}


void Rem(bigInt dst, bigInt b_u, bigInt b_v) {
    // u 2*(2048>>6)
    // v (2048>>6)
    // Allocate data

    uint32* u = malloc(2*(2048>>3) + (32>>3));
    for (int i = 0; i < 2*(2048>>6); i++) {
        ((uint64*)u)[i] = b_u[i];
    }
    ((uint64*)u)[2*(2048>>6)] = 0;
    
    uint32* v = malloc((2048>>3));
    for (int i = 0; i < (2048>>6); i++) {
        ((uint64*)v)[i] = b_v[i];
    }

    int s = Clz((bigInt)v, (2048>>6));
    for (int i = 0; i < s; i++) {
        LeftShift((bigInt)v, (2048>>6));
        LeftShift((bigInt)u, 2*(2048>>6));
    }

    uint32* r = malloc((2048>>3));
    for (int i = (2048>>5); i >= 0; i--) {
        RemHelper(r, u+i, v);
    }
    free(r);



    // Normalize
    // Initialize J
    free(u);
    free(v);
}


/*

int Cmp(bigInt a, bigInt b) {
    for (int i = 2*(2048>>6)-1; i >= 0; i--) {
        if (a[i] == b[i]) {
            continue;
        }
        return a[i] > b[i] ? 1 : -1;
    }
    return 0;
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
