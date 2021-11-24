package main

import (
    "crypto/rsa"
    "crypto/rand"
    "os"
)

func main() {
    key, err := rsa.GenerateKey(rand.Reader, 2048)
    if err != nil { panic("Failed to generate key") }

    fp, err := os.Create("Dp")
    if err != nil { panic("Failed to open Dp") }
    fp.Write(key.Precomputed.Dp.Bytes())
    fp.Close()

    fp, err = os.Create("Dq")
    if err != nil { panic("Failed to open Dq") }
    fp.Write(key.Precomputed.Dq.Bytes())
    fp.Close()

    fp, err = os.Create("N")
    if err != nil { panic("Failed to open N") }
    fp.Write(key.PublicKey.N.Bytes())
    fp.Close()
}

