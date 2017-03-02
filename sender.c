#include "sender.h"
#define DEBUG 0

struct timeval *sender_get_timeout_timeval() {

    struct timeval *timeout;
    timeout = malloc(sizeof(struct timeval));
    gettimeofday(timeout, NULL);
    timeout->tv_usec += 0.1 * 1000 * 1000;
    if (timeout->tv_usec >= 1000000) {
        timeout->tv_sec++;
        timeout->tv_usec -= 1000000;
    }
    return timeout;
}

//find the receiver by id
RecvInfo * find_recv(Sender *sender, int dst_id, int flag) {
    LLnode * receiver_head;
    LLnode * receiver_iterator;

    receiver_iterator = receiver_head = sender->receiver_list_head;

    if(receiver_head != NULL) {
        do {
            RecvInfo* recv = (RecvInfo*) receiver_iterator->value;
            if( recv->recv_id == dst_id ) {
                return recv;
            }
            receiver_iterator = receiver_iterator->next;
        } while (receiver_iterator != receiver_head);
    }

    if(flag == CREATE) {
        RecvInfo * recvinfo = (RecvInfo *) malloc(sizeof(RecvInfo));
        recvinfo->recv_id = dst_id;
        recvinfo->SWS = MAX_FRAME;
        recvinfo->LAR = BEGNUM;
        recvinfo->LFS = BEGNUM;
        int i;
        for (i = 0; i < MAX_FRAME; ++i) {
            recvinfo->buffer[i] = NULL;
        }
        ll_append_node(&(sender->receiver_list_head), recvinfo);
        return recvinfo;
    } else {
        return NULL;
    }

}

//insert the frame to the receiver's frame list
void insert_recv_list(Sender *sender, int dst_id, Frame * frame, struct timeval *timeout) {
    RecvInfo * recv = find_recv(sender, dst_id, NO_CREATE);
    if(recv == NULL) {
        fprintf(stderr, "error\n");
        return;
    }
    Outgoing_Frames * out_frame = (Outgoing_Frames *)malloc(sizeof(Outgoing_Frames));
    out_frame->timeout = timeout;
    out_frame->frame = frame;
    unsigned int seq = out_frame->frame->seq;
    if(seq < recv->LAR) {
      seq += 256;
    }
    recv->buffer[seq - recv->LAR - 1] = out_frame;
}

//get the next seq
unsigned char get_next_seq(Sender *sender, int dst_id) {
    RecvInfo * recv = find_recv(sender, dst_id, NO_CREATE);
    unsigned int LFS = recv->LFS;
    if(recv->LFS < recv->LAR) {
      LFS += 256;
    }
    if((LFS - recv->LAR) < (recv->SWS - 1)) {
	    recv->LFS++;
        return (recv->LFS);
    }else {
        fprintf(stderr, "error\n");
        return 1;
    }
}

//determine whether the buffer is full (in this case, more than 8)
int buffer_full(Sender *sender, int dst_id) {
    RecvInfo * recv = find_recv(sender, dst_id, CREATE);
    unsigned int LFS = recv->LFS;
    if(recv->LFS < recv->LAR) {
      LFS += 256;
    }
    if((LFS - recv->LAR) < (recv->SWS - 1)) {
        return 0;
    }else {
        return 1;
    }
}
 
void init_sender(Sender * sender, int id)
{
    //TODO: You should fill in this function as necessary
    sender->send_id = id;
    sender->input_cmdlist_head = NULL;
    sender->input_framelist_head = NULL;
    sender->receiver_list_head = NULL;
}

//get the next expiring timeval
struct timeval * sender_get_next_expiring_timeval(Sender * sender)
{
    LLnode* head = sender->receiver_list_head;
    if (head == NULL) {
        return NULL;
    } else {
        struct timeval *min_time_spec;
        min_time_spec = malloc(sizeof(struct timeval));
	
        gettimeofday(min_time_spec, NULL);
	
        min_time_spec->tv_sec += 10;
        LLnode* iterator = head;
        do {
            RecvInfo * recv = (RecvInfo *)(iterator->value);
            int i = 0;
            for (i = 0; i < 8; ++i) {
                if(recv->buffer[i] != NULL && timeval_usecdiff(recv->buffer[i]->timeout, min_time_spec) > 0) {
                    min_time_spec->tv_sec = recv->buffer[i]->timeout->tv_sec;
                    min_time_spec->tv_usec = recv->buffer[i]->timeout->tv_usec;
                }
            }
            
            iterator = iterator->next;
        } while (iterator != head);
    	return min_time_spec;
    }
    return NULL;
}


void handle_incoming_acks(Sender * sender,
                          LLnode ** outgoing_frames_head_ptr)
{
    //TODO: Suggested steps for handling incoming ACKs
    //    1) Dequeue the ACK from the sender->input_framelist_head
    //    2) Convert the char * buffer to a Frame data type
    //    3) Check whether the frame is corrupted
    //    4) Check whether the frame is for this sender
    //    5) Do sliding window protocol for sender/receiver pair
<<<<<<< HEAD
    LLnode* ack_head = ack_iterator = sender->input_framelist_head;
    if(ack_head != NULL) {
        do {
            (Frame *) in_frame = convert_char_to_frame((char *)ack_iterator->value);
            //unsigned char ack = frame->ack;
            int dst_id = in_frame->dst_id;

            if(dst_id == sender->send_id) {
                LLnode * receiver_head;
                LLnode * receiver_iterator;

                receiver_iterator = receiver_head = sender->receiver_list_head;
                if (receiver_head != NULL) {
                    do {
                        (RecvInfo*) recv = (RecvInfo*) receiver_iterator->value;
                        if(recv->recv_id == in_frame->src_id) {
                            LLnode * receiver_frame_head;
                            LLnode * receiver_frame_iterator;

                            receiver_frame_iterator = receiver_frame_head = recv->outgoing_frames_head;
                            if (receiver_frame_head != NULL) {
                                do {
                                    (Frame*) out_frame = (Frame*) receiver_frame_iterator->value;
                                    if(out_frame->seq == in_frame->seq) {
                                        if(flag == 0) {
                                            //re-send
                                        }  
                                        printf("<Feedback of %d>:[%d]\n", in_frame->src_id, in_frame->seq);

                                    }
                                    receiver_iterator = receiver_iterator->next;
                                } while (receiver_iterator != receiver_head)
                            }


                        }
                        receiver_iterator = receiver_iterator->next;
                    } while (receiver_iterator != receiver_head)
                }
            }

            

            

        }while(iterator != ack_head)
    }   
=======
    int input_frame_length = ll_get_length(sender-> input_framelist_head);
    while (input_frame_length > 0) {
        //Pop a node off and update the input_cmd_length
	    LLnode * ll_input_frame_node = ll_pop_node(&sender->input_framelist_head);
        input_frame_length = ll_get_length(sender->input_framelist_head);
        //Cast to Cmd type and free up the memory for the node
        char *charbuf = (char *) ll_input_frame_node->value;
    	Frame * in_frame = convert_char_to_frame(charbuf);
        free(charbuf);
        free(ll_input_frame_node);

        unsigned char crc = in_frame->CRC;
        in_frame->CRC = 0;
        char * check_crc = convert_frame_to_char(in_frame);
        if(crc == CRC8(check_crc, strlen(check_crc))) {
            int dst_id = in_frame->dst_id;

            if(dst_id == sender->send_id) {
                //printf("<Feedback of %d>:[%d]\n", in_frame->src_id, in_frame->seq);
                
                RecvInfo* recv = find_recv(sender, in_frame->src_id, NO_CREATE);
                if (recv == NULL) {
                    continue;
                }
                int i;
                for (i = 0; i < MAX_FRAME; ++i) {
                    if(recv->buffer[i] != NULL && recv->buffer[i]->frame->dst_id == in_frame->src_id) {
                        if(recv->buffer[i]->frame->seq == in_frame->seq) {
                            if(in_frame->flag == 1) {
                                free(recv->buffer[i]->timeout);
                                recv->buffer[i]->timeout = sender_get_timeout_timeval();
                                char * outgoing_charbuf = convert_frame_to_char(recv->buffer[i]->frame);
                                ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
                            } else {
                                free(recv->buffer[i]->timeout);
                                free(recv->buffer[i]);
                                recv->buffer[i] = NULL;

                                if(recv->LAR < in_frame->ack) {
                                    int j;
                                    for(j = 0; j < MAX_FRAME; ++j) {
                                        if(j < (MAX_FRAME - (in_frame->ack - recv->LAR))) {
                                            recv->buffer[j] = recv->buffer[j+(in_frame->ack - recv->LAR)];
                                        }else {
                                            recv->buffer[j] = NULL;
                                        }
                                    }
                                    recv->LAR = in_frame->ack;
                                }else if(recv->LAR > 240 && in_frame->ack < 10) {
                                    int j;
                                    for(j = 0; j < MAX_FRAME; ++j) {
                                        if(j < (MAX_FRAME - (in_frame->ack + 256 - recv->LAR))) {
                                            recv->buffer[j] = recv->buffer[j+(in_frame->ack + 256 - recv->LAR)];
                                        }else {
                                            recv->buffer[j] = NULL;
                                        }
                                    }
                                    recv->LAR = in_frame->ack;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
>>>>>>> 5e574a9a662fd08627462f57f278ade20975a42c
}

void handle_input_cmds(Sender * sender,
                       LLnode ** outgoing_frames_head_ptr)
{
    //TODO: Suggested steps for handling input cmd
    //    1) Dequeue the Cmd from sender->input_cmdlist_head
    //    2) Convert to Frame
    //    3) Set up the frame according to the sliding window protocol
    //    4) Compute CRC and add CRC to Frame

    int input_cmd_length = ll_get_length(sender->input_cmdlist_head);
    
    LLnode *tmp_store = NULL;
        
    //Recheck the command queue length to see if stdin_thread dumped a command on us
    input_cmd_length = ll_get_length(sender->input_cmdlist_head);
    while (input_cmd_length > 0)
    {
        //Pop a node off and update the input_cmd_length
        LLnode * ll_input_cmd_node = ll_pop_node(&sender->input_cmdlist_head);
        input_cmd_length = ll_get_length(sender->input_cmdlist_head);

        //Cast to Cmd type and free up the memory for the node
        Cmd * outgoing_cmd = (Cmd *) ll_input_cmd_node->value;
        int full = buffer_full(sender, outgoing_cmd->dst_id);
        if(full == 0) {
            free(ll_input_cmd_node);
            unsigned char seq = get_next_seq(sender, outgoing_cmd->dst_id);
            

            //DUMMY CODE: Add the raw char buf to the outgoing_frames list
            //NOTE: You should not blindly send this message out!
            //      Ask yourself: Is this message actually going to the right receiver (recall that default behavior of send is to broadcast to all receivers)?
            //                    Does the receiver have enough space in in it's input queue to handle this message?
            //                    Were the previous messages sent to this receiver ACTUALLY delivered to the receiver?
    	    int msg_length = strlen(outgoing_cmd->message);
            if (msg_length > FRAME_PAYLOAD_SIZE)
            {
                //Do something about messages that exceed the frame size
               
                Cmd * new_cmd = (Cmd *)malloc(sizeof(Cmd));
                new_cmd->src_id = outgoing_cmd->src_id;
                new_cmd->dst_id = outgoing_cmd->dst_id;
                char * charbuf = outgoing_cmd->message;
                new_cmd->message = malloc(sizeof(char) * (msg_length));
                strcpy(new_cmd->message, charbuf+FRAME_PAYLOAD_SIZE);
                ll_insert_at_first_node(&(sender->input_cmdlist_head), new_cmd);
            }
            //This is probably ONLY one step you want
            Frame * outgoing_frame = (Frame *) malloc (sizeof(Frame));
<<<<<<< HEAD
            outgoing_frame -> seq = 1;
            outgoing_frame->src_id = outgoing_cmd->src_id;
            outgoing_frame->dst_id = outgoing_cmd->dst_id;
            strcpy(outgoing_frame->data, outgoing_cmd->message);

=======
	        outgoing_frame->src_id = outgoing_cmd->src_id;
            outgoing_frame->dst_id = outgoing_cmd->dst_id;

            outgoing_frame->seq = seq;
            strncpy(outgoing_frame->data, outgoing_cmd->message, FRAME_PAYLOAD_SIZE);
            outgoing_frame->data[FRAME_PAYLOAD_SIZE] = 0;
    		outgoing_frame->data[FRAME_PAYLOAD_SIZE+1] = 0;
    		outgoing_frame->data[FRAME_PAYLOAD_SIZE+2] = 0;
    		outgoing_frame->data[FRAME_PAYLOAD_SIZE+3] = 0;
    		outgoing_frame->CRC = 0;
            outgoing_frame->ack = 0;
            outgoing_frame->flag = 0;
            char * char_for_crc = convert_frame_to_char(outgoing_frame);
            outgoing_frame->CRC = CRC8(char_for_crc, strlen(char_for_crc));

            free(char_for_crc);
>>>>>>> 5e574a9a662fd08627462f57f278ade20975a42c
            //At this point, we don't need the outgoing_cmd
            free(outgoing_cmd->message);
            free(outgoing_cmd);

            //Convert the message to the outgoing_charbuf
            char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
            //Frame * test = convert_char_to_frame(outgoing_charbuf);
            struct timeval *timeout = sender_get_timeout_timeval();
            insert_recv_list(sender, outgoing_frame->dst_id, outgoing_frame, timeout);

            ll_append_node(outgoing_frames_head_ptr,
                           outgoing_charbuf);
            
        } else {
            ll_append_node(&tmp_store, outgoing_cmd);
        }
    }
    int tmp_length = ll_get_length(tmp_store);
    while (tmp_length > 0) {
	    LLnode *tmp = ll_pop_node(&tmp_store);
        ll_append_node(&(sender->input_cmdlist_head), tmp->value);
        free(tmp);
        tmp_length = ll_get_length(tmp_store);
    }
}


void handle_timedout_frames(Sender * sender,
                            LLnode ** outgoing_frames_head_ptr)
{
    //TODO: Suggested steps for handling timed out datagrams
    //    1) Iterate through the sliding window protocol information you maintain for each receiver
    //    2) Locate frames that are timed out and add them to the outgoing frames
    //    3) Update the next timeout field on the outgoing frames

    LLnode * receiver_head;
    LLnode * receiver_iterator;

    receiver_iterator = receiver_head = sender->receiver_list_head;

    if (receiver_head != NULL) {
        struct timeval curr_timeval;
        do {
            RecvInfo* recv = (RecvInfo*) receiver_iterator->value;
            int i;
            for (i = 0; i < MAX_FRAME; ++i) {
                if(recv->buffer[i] != NULL) {
                    gettimeofday(&curr_timeval, NULL);
                    if(timeval_usecdiff(&curr_timeval, recv->buffer[i]-> timeout) < 0) {
                        free(recv->buffer[i]->timeout);
                        recv->buffer[i]->timeout = sender_get_timeout_timeval();
                        char * outgoing_charbuf = convert_frame_to_char(recv->buffer[i]->frame);
                        ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
                    }
                }
            }
        } while (receiver_iterator != receiver_head);
    }
    return;
}


void * run_sender(void * input_sender)
{    
    struct timespec   time_spec;
    struct timeval    curr_timeval;
    const int WAIT_SEC_TIME = 0;
    const long WAIT_USEC_TIME = 100000;
    Sender * sender = (Sender *) input_sender;    
    LLnode * outgoing_frames_head;
    struct timeval * expiring_timeval;
    long sleep_usec_time, sleep_sec_time;
    
    //This incomplete sender thread, at a high level, loops as follows:
    //1. Determine the next time the thread should wake up
    //2. Grab the mutex protecting the input_cmd/inframe queues
    //3. Dequeues messages from the input queue and adds them to the outgoing_frames list
    //4. Releases the lock
    //5. Sends out the messages

    pthread_cond_init(&sender->buffer_cv, NULL);
    pthread_mutex_init(&sender->buffer_mutex, NULL);
    while(1)
    {    
        outgoing_frames_head = NULL;

        //Get the current time
        gettimeofday(&curr_timeval, 
                     NULL);

        //time_spec is a data structure used to specify when the thread should wake up
        //The time is specified as an ABSOLUTE (meaning, conceptually, you specify 9/23/2010 @ 1pm, wakeup)
        time_spec.tv_sec  = curr_timeval.tv_sec;
        time_spec.tv_nsec = curr_timeval.tv_usec * 1000;

        //Check for the next event we should handle
        expiring_timeval = sender_get_next_expiring_timeval(sender);

        //Perform full on timeout
        if (expiring_timeval == NULL)
        {
            time_spec.tv_sec += WAIT_SEC_TIME;
            time_spec.tv_nsec += WAIT_USEC_TIME * 1000;
        }
        else
        {
            //Take the difference between the next event and the current time
            sleep_usec_time = timeval_usecdiff(&curr_timeval,
                                               expiring_timeval);

            //Sleep if the difference is positive
            if (sleep_usec_time > 0)
            {
                sleep_sec_time = sleep_usec_time/1000000;
                sleep_usec_time = sleep_usec_time % 1000000;   
                time_spec.tv_sec += sleep_sec_time;
                time_spec.tv_nsec += sleep_usec_time*1000;
            }   
        }

        //Check to make sure we didn't "overflow" the nanosecond field
        if (time_spec.tv_nsec >= 1000000000)
        {
            time_spec.tv_sec++;
            time_spec.tv_nsec -= 1000000000;
        }
	    
        //*****************************************************************************************
        //NOTE: Anything that involves dequeing from the input frames or input commands should go 
        //      between the mutex lock and unlock, because other threads CAN/WILL access these structures
        //*****************************************************************************************
        pthread_mutex_lock(&sender->buffer_mutex);

        

        //Check whether anything has arrived
        int input_cmd_length = ll_get_length(sender->input_cmdlist_head);
        int inframe_queue_length = ll_get_length(sender->input_framelist_head);
        
        //Nothing (cmd nor incoming frame) has arrived, so do a timed wait on the sender's condition variable (releases lock)
        //A signal on the condition variable will wakeup the thread and reaquire the lock
        if (input_cmd_length == 0 &&
            inframe_queue_length == 0)
        {
            
            pthread_cond_timedwait(&sender->buffer_cv, 
                                   &sender->buffer_mutex,
                                   &time_spec);
        }
        //Implement this
        handle_incoming_acks(sender,
                             &outgoing_frames_head);

        //Implement this
        handle_input_cmds(sender,
                          &outgoing_frames_head);

        pthread_mutex_unlock(&sender->buffer_mutex);


        //Implement this
        handle_timedout_frames(sender, &outgoing_frames_head);

        //CHANGE THIS AT YOUR OWN RISK!
        //Send out all the frames
        int ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        
        while(ll_outgoing_frame_length > 0)
        {
            LLnode * ll_outframe_node = ll_pop_node(&outgoing_frames_head);
            char * char_buf = (char *)  ll_outframe_node->value;

            //Don't worry about freeing the char_buf, the following function does that
            send_msg_to_receivers(char_buf);

            //Free up the ll_outframe_node
            free(ll_outframe_node);

            ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        }
    }
    pthread_exit(NULL);
    return 0;
}
