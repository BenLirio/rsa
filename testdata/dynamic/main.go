package main

import (
    "math/big"
    "math/rand"
    "os"
    "strings"
    "encoding/binary"
    "fmt"
)

type TestCase struct {
    A *big.Int
    B *big.Int
}

func rev(bs []byte) []byte {
    for i := 0; i < len(bs)/2; i++ {
        bs[i], bs[len(bs)-i-1] = bs[len(bs)-i-1], bs[i]
    }
    return bs
}

func (tc TestCase) Marshal() ([]byte) {
    out := []byte{}

    bs := make([]byte, 4)
    binary.LittleEndian.PutUint32(bs, uint32(len(tc.A.Bytes())))
    out = append(out, bs...)
    out = append(out, rev(tc.A.Bytes())...)

    bs = make([]byte, 4)
    binary.LittleEndian.PutUint32(bs, uint32(len(tc.B.Bytes())))
    out = append(out, bs...)
    out = append(out, rev(tc.B.Bytes())...)

    C := new(big.Int).Mul(tc.A, tc.B)
    bs = make([]byte, 4)
    binary.LittleEndian.PutUint32(bs, uint32(len(C.Bytes())))
    out = append(out, bs...)
    out = append(out, rev(C.Bytes())...)

    return out

}

var MAX_BITS int = 1<<15
var N_BITS int = 1<<13;

func GenTestCase() TestCase {
    var AStrBuilder strings.Builder
    var BStrBuilder strings.Builder
    base := 16
    //ALen := rand.Intn(MAX_BITS/4)
    //BLen := rand.Intn(MAX_BITS/4)
    ALen := N_BITS
    BLen := N_BITS
    for i := 0; i < ALen; i++ {
        fmt.Fprintf(&AStrBuilder, "%x", rand.Intn(base))
    }
    for i := 0; i < BLen; i++ {
        fmt.Fprintf(&BStrBuilder, "%x", rand.Intn(base))
    }

    A, ok := new(big.Int).SetString(AStrBuilder.String(), base)
    if !ok { panic("Failed to convert string") }
    B, ok := new(big.Int).SetString(BStrBuilder.String(), base)
    if !ok { panic("Failed to convert string") }
    return TestCase{A, B}
}

var NUM_TESTS int = 32

func main() {
    fp, err := os.Create("testcases")
    if err != nil { panic(err) }

    for i := 0; i < NUM_TESTS; i++ {
        tc := GenTestCase()
        fp.Write(tc.Marshal())
    }
    fp.Close()
}
