#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <math.h>
#include <sys/time.h>

#define MAX_COMMAND_LENGTH 16
#define AUTOMATED_FILENAME 512
#define MAX_FRAME 8
#define CREATE 0
#define NO_CREATE 1
#define BEGNUM 0
typedef unsigned char uchar_t;

//System configuration information
struct SysConfig_t
{
    float drop_prob;
    float corrupt_prob;
    unsigned char automated;
    char automated_file[AUTOMATED_FILENAME];
};
typedef struct SysConfig_t  SysConfig;

//Command line input information
struct Cmd_t
{
    uint16_t src_id;
    uint16_t dst_id;
    char * message;
};
typedef struct Cmd_t Cmd;

//Linked list information
enum LLtype 
{
    llt_string,
    llt_frame,
    llt_integer,
    llt_head
} LLtype;

struct LLnode_t
{
    struct LLnode_t * prev;
    struct LLnode_t * next;
    enum LLtype type;

    void * value;
};
typedef struct LLnode_t LLnode;


//Receiver and sender data structures
struct Receiver_t
{
    //DO NOT CHANGE:
    // 1) buffer_mutex
    // 2) buffer_cv
    // 3) input_framelist_head
    // 4) recv_id
    pthread_mutex_t buffer_mutex;
    pthread_cond_t buffer_cv;
    LLnode * input_framelist_head;
    LLnode * sender_list_head;
    int recv_id;
};

struct Sender_t
{
    //DO NOT CHANGE:
    // 1) buffer_mutex
    // 2) buffer_cv
    // 3) input_cmdlist_head
    // 4) input_framelist_head
    // 5) send_id
    pthread_mutex_t buffer_mutex;
    pthread_cond_t buffer_cv;    
    LLnode * input_cmdlist_head;
    LLnode * input_framelist_head;
    int send_id;
    LLnode * receiver_list_head;
};

enum SendFrame_DstType 
{
    ReceiverDst,
    SenderDst
} SendFrame_DstType ;

typedef struct Sender_t Sender;
typedef struct Receiver_t Receiver;


#define MAX_FRAME_SIZE 64

//TODO: You should change this!
//Remember, your frame can be AT MOST 64 bytes!
<<<<<<< HEAD
#define FRAME_PAYLOAD_SIZE 54
struct Frame_t
{
    unsigned char seq;
    unsigned char flag;
    int dst_id;
    int src_id;
    char data[FRAME_PAYLOAD_SIZE];
};
=======
#define FRAME_PAYLOAD_SIZE 48
#define TRUE_PLAYLOAD_SIZE 52
struct Frame_t
{
    unsigned char seq;
    unsigned char ack;
    int dst_id;
    int src_id;
    unsigned char CRC;
    unsigned char flag;

    char data[TRUE_PLAYLOAD_SIZE];
}__attribute__((packed));
>>>>>>> 5e574a9a662fd08627462f57f278ade20975a42c
typedef struct Frame_t Frame;
/*
void copy_frame(Frame *a, Frame *b) {
    a->seq = b->seq;
    a->flag = b->flag;
    a->dst_id = b->dst_id;
    a->src_id = b->src_id;
}*/
struct Outgoing_Frames_t {
    struct timeval *timeout;
    Frame * frame;

};
typedef struct Outgoing_Frames_t Outgoing_Frames;

struct RecvInfo_t {
  int recv_id;
  unsigned char SWS;
  unsigned char LAR;
  unsigned char LFS;
  Outgoing_Frames *buffer[MAX_FRAME];
};
typedef struct RecvInfo_t RecvInfo;



struct SenderInfo_t {
  int sender_id;
  unsigned char SWS;
  unsigned char LFR;
  unsigned char LAF;
  Frame *buffer[MAX_FRAME];
};
typedef struct SenderInfo_t SenderInfo;




void copy_frame(Frame *a, Frame *b) {
    a->seq = b->seq;
    a->flag = b->flag;
    a->dst_id = b->dst_id;
    a->src_id = b->src_id;
}


struct RecvInfo_t {
  int recv_id;
  unsigned char seq;
  unsigned char ack;
  unsigned char SWS;
  unsigned char LAR;
  unsigned char LFS;
  LLnode * outgoing_frames_head;
};
typedef struct RecvInfo_t RecvInfo;
//Declare global variables here
//DO NOT CHANGE: 
//   1) glb_senders_array
//   2) glb_receivers_array
//   3) glb_senders_array_length
//   4) glb_receivers_array_length
//   5) glb_sysconfig
//   6) CORRUPTION_BITS
Sender * glb_senders_array;
Receiver * glb_receivers_array;
int glb_senders_array_length;
int glb_receivers_array_length;
SysConfig glb_sysconfig;
int CORRUPTION_BITS;
#endif 
