Create register file and two RAM banks.
Have protocol talk to register file and RAM banks.
Let the host application worry about async nature of tearing.

Three modes:
    Data
    Control
    Query

Need hardware flow control, not going to use DMA for RX. (Maybe DMA for TX.)