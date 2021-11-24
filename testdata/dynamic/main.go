package main

import (
    "math/big"
    "os"
    "encoding/binary"
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

func GenTestCase() TestCase {
    tc := TestCase{
        A: new(big.Int).SetUint64(uint64(1<<32)),
        B: new(big.Int).SetUint64(uint64(1<<63)),
    }
    return tc
}


func main() {
    fp, err := os.Create("testcases")
    if err != nil { panic(err) }
    tc := GenTestCase()
    fp.Write(tc.Marshal())
    fp.Close()
}
