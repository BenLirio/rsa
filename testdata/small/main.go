package main

import (
    "math/big"
    "encoding/binary"
    "os"
)

type Numbers [2]string

var Cases []Numbers = []Numbers{
    Numbers{"ffffffff", "ffffffff"},
    Numbers{"ffffffffffffffff", "ffffffffffffffff"},
    Numbers{"ffffffffffffffff", "1"},
    Numbers{"1", "244"},
    Numbers{"1234123412421532325234122353429384729364893426394", "3284287364982634728364782634827631209378129836124872364"},
    Numbers{"12341234124215323252abcabcaaaaaaaaaaaa32333333333333333333322222222222222222223bbbbbbbbbbbb34122353429384729364893426394", "32842873649826347283647826348276312093781298361248723838232222222222bbbbbbbbbbb64"},
}

func rev(b []byte) []byte {
    for i := 0; i < len(b)/2; i++ {
        b[i], b[len(b)-i-1] = b[len(b)-i-1], b[i]
    }
    return b
}

func main() {
    fp, err := os.Create("numbers")
    if err != nil { panic(err) }
    for i := 0; i < len(Cases); i++ {
        a, ok := new(big.Int).SetString(Cases[i][0], 16)
        if ok != true { panic("SetString Failed") }
        err = binary.Write(fp, binary.LittleEndian, uint32(len(a.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(a.Bytes()))


        b, ok := new(big.Int).SetString(Cases[i][1], 16)
        if ok != true { panic("SetString Failed") }
        err = binary.Write(fp, binary.LittleEndian, uint32(len(b.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(b.Bytes()))

        c := new(big.Int).Mul(a, b)
        err = binary.Write(fp, binary.LittleEndian, uint32(len(c.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(c.Bytes()))

    }
    fp.Close()
}
