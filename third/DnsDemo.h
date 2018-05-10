#pragma pack(push, 1)

struct DNSHeader {
    //Transaction ID in 2 bytes
    unsigned short Transaction_ID;

    //Flags in 2 bytes
    unsigned char RD:1;             //Recursion Desired
    unsigned char TC:1;             //TrunCation means longer than 512 bytes
    unsigned char AA:1;             //Authoritative Answer 
    unsigned char opcode:4;         //0 for standard, 1 for reverse, 2 for status request
    unsigned char QR:1;             //0 for request 1 for answer

    unsigned char rcode:4;          //return code
    unsigned char zero:3;           //supposed to be 0
    unsigned char RA:1;             //Recursion Available

    //count
    unsigned short Questions;       //as the name
    unsigned short AnswerRRs;
    unsigned short AuthotiryRRs;
    unsigned short AdditionalRRs;
};

#pragma pack(pop)