# raspberryProtocol
School project to develop protocol and use it to transfer data between two raspberry pi 4
catch is to not use spi, uart or another ready protocol to transfer data. Only normal gpio pins are allowed
data need be fully transferred even wires connection lost for while.
CRC check needed do also in protocol

Now this program send and save under 50 bytes message

todo:
slice input data in multiple packets

