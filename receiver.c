#include "receiver.h"

void init_receiver(Receiver * receiver,
                   int id)
{
    receiver->recv_id = id;
    receiver->input_framelist_head = NULL;
}


SenderInfo * find_sender(Receiver *receiver, int src_id, int flag) {
    LLnode * sender_head;
    LLnode * sender_iterator;

    sender_iterator = sender_head = receiver
    ->sender_list_head;

    if(sender_head != NULL) {
        do {
            SenderInfo* sender = (SenderInfo*) sender_iterator->value;
            if( sender->sender_id == src_id ) {
                return sender;
            }
            sender_iterator = sender_iterator->next;
        } while (sender_iterator != sender_head);
    } 

    if(flag == CREATE) {
        SenderInfo * senderinfo = (SenderInfo *) malloc(sizeof(SenderInfo));
        senderinfo->sender_id = src_id;
        senderinfo->SWS = MAX_FRAME;
        senderinfo->LFR = BEGNUM;
        senderinfo->LAF = BEGNUM;
        senderinfo->LAF += MAX_FRAME;
        senderinfo->LAF -= 1;
        int i;
        for(i = 0; i < MAX_FRAME; ++i) {
            senderinfo->buffer[i] = NULL;
        }
        ll_append_node(&(receiver->sender_list_head), senderinfo);
        return senderinfo;
    } else {
        return NULL;
    }

}


void handle_incoming_msgs(Receiver * receiver,
                          LLnode ** outgoing_frames_head_ptr)
{
    //TODO: Suggested steps for handling incoming frames
    //    1) Dequeue the Frame from the sender->input_framelist_head
    //    2) Convert the char * buffer to a Frame data type
    //    3) Check whether the frame is corrupted
    //    4) Check whether the frame is for this receiver
    //    5) Do sliding window protocol for sender/receiver pair

    int incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
    while (incoming_msgs_length > 0)
    {
        //Pop a node off the front of the link list and update the count
        LLnode * ll_inmsg_node = ll_pop_node(&receiver->input_framelist_head);
        incoming_msgs_length = ll_get_length(receiver->input_framelist_head);

        //DUMMY CODE: Print the raw_char_buf
        //NOTE: You should not blindly print messages!
        //      Ask yourself: Is this message really for me?
        //                    Is this message corrupted?
        //                    Is this an old, retransmitted message?           
        char * raw_char_buf = (char *) ll_inmsg_node->value;
        Frame * inframe = convert_char_to_frame(raw_char_buf);
        unsigned char crc = inframe->CRC;
        inframe->CRC = 0;
        char * check_crc = convert_frame_to_char(inframe);
        int corrupted;
        if(crc == CRC8(check_crc, strlen(check_crc))) {
            corrupted = 0;
        } else{
            corrupted = 1;
        }

        //printf("is it corrupted? %d\n", corrupted);

        
        //Free raw_char_buf
        free(raw_char_buf); 
        
        if(inframe->dst_id == receiver->recv_id) {
            //////////handle corrput;
<<<<<<< HEAD
            Frame * ackframe = malloc(sizeof(Frame))
            ackframe->dst_id = ackframe->src_id;
            ackframe->src_id = ackframe->dst_id;
            ackframe->flag = 1;
            ll_append_node(outgoing_frames_head_ptr, ackframe);
            printf("<RECV_%d>: with seq is %d, and data[%s]\n", receiver->recv_id, inframe->seq,inframe->data);
=======
            SenderInfo *sender = find_sender(receiver, inframe->src_id, CREATE);
            unsigned int LAF = sender->LAF;
	        unsigned int seq = inframe->seq;
            if (LAF < sender->LFR) {
     
                LAF += 256;
            }

    	    if(seq < sender -> LFR) {
    	      seq += 256;
    	    }
            Frame * ackframe = (Frame *) malloc(sizeof(Frame));

            ackframe->dst_id = inframe->src_id;
            ackframe->src_id = inframe->dst_id;
            ackframe->seq = inframe->seq; 
            ackframe->flag = corrupted;  
            if(!corrupted && seq > sender->LFR && seq <= LAF) {
	        sender->buffer[seq - sender->LFR - 1] = inframe;
                if(sender->buffer[0] != NULL) {
                    unsigned char count = 0;
                    while(sender->buffer[count] != NULL && count < MAX_FRAME) {
                        count++;
                    }
                    int j;
		            for(j = 0; j < count; ++j) {
                        printf("<RECV_%d>:[%s]\n", receiver->recv_id, sender->buffer[j]->data);
			            //printf("<RECV_%d>: with seq is %u, and data[%s]\n", receiver->recv_id, sender->buffer[j]->seq,);
			        }
                    for(j = 0; j < MAX_FRAME; ++j) {
                        if(j < (MAX_FRAME - count)) {
                            sender->buffer[j] = sender->buffer[j+count];
                        }else {
                            sender->buffer[j] = NULL;
                        }
                    }
                    sender->LFR += count;
                    sender->LAF += count;

                }
               // printf("<RECV_%d>: with seq is %u, and data[%s]\n", receiver->recv_id, inframe->seq, inframe->data);
            }

            ackframe->ack = sender->LFR;
            ackframe->CRC = 0;

            char * char_for_crc = convert_frame_to_char(ackframe);
            ackframe->CRC = CRC8(char_for_crc, strlen(char_for_crc));

            free(char_for_crc);
            char * ackframe_charbuf = convert_frame_to_char(ackframe);
            
            ll_append_node(outgoing_frames_head_ptr, ackframe_charbuf);
            //printf("<RECV_%d>: with seq is %u, and data[%s]\n", receiver->recv_id, inframe->seq,inframe->data);
>>>>>>> 5e574a9a662fd08627462f57f278ade20975a42c
        }

	// free(inframe);
        free(ll_inmsg_node);
    }
}

void * run_receiver(void * input_receiver)
{    
    struct timespec   time_spec;
    struct timeval    curr_timeval;
    const int WAIT_SEC_TIME = 0;
    const long WAIT_USEC_TIME = 100000;
    Receiver * receiver = (Receiver *) input_receiver;
    LLnode * outgoing_frames_head;


    //This incomplete receiver thread, at a high level, loops as follows:
    //1. Determine the next time the thread should wake up if there is nothing in the incoming queue(s)
    //2. Grab the mutex protecting the input_msg queue
    //3. Dequeues messages from the input_msg queue and prints them
    //4. Releases the lock
    //5. Sends out any outgoing messages

    pthread_cond_init(&receiver->buffer_cv, NULL);
    pthread_mutex_init(&receiver->buffer_mutex, NULL);

    while(1)
    {    
        //NOTE: Add outgoing messages to the outgoing_frames_head pointer
        outgoing_frames_head = NULL;
        gettimeofday(&curr_timeval, 
                     NULL);

        //Either timeout or get woken up because you've received a datagram
        //NOTE: You don't really need to do anything here, but it might be useful for debugging purposes to have the receivers periodically wakeup and print info
        time_spec.tv_sec  = curr_timeval.tv_sec;
        time_spec.tv_nsec = curr_timeval.tv_usec * 1000;
        time_spec.tv_sec += WAIT_SEC_TIME;
        time_spec.tv_nsec += WAIT_USEC_TIME * 1000;
        if (time_spec.tv_nsec >= 1000000000)
        {
            time_spec.tv_sec++;
            time_spec.tv_nsec -= 1000000000;
        }

        //*****************************************************************************************
        //NOTE: Anything that involves dequeing from the input frames should go 
        //      between the mutex lock and unlock, because other threads CAN/WILL access these structures
        //*****************************************************************************************
        pthread_mutex_lock(&receiver->buffer_mutex);

        //Check whether anything arrived
        int incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
        if (incoming_msgs_length == 0)
        {
            //Nothing has arrived, do a timed wait on the condition variable (which releases the mutex). Again, you don't really need to do the timed wait.
            //A signal on the condition variable will wake up the thread and reacquire the lock
            pthread_cond_timedwait(&receiver->buffer_cv, 
                                   &receiver->buffer_mutex,
                                   &time_spec);
        }

        handle_incoming_msgs(receiver,
                             &outgoing_frames_head);

        pthread_mutex_unlock(&receiver->buffer_mutex);
        
        //CHANGE THIS AT YOUR OWN RISK!
        //Send out all the frames user has appended to the outgoing_frames list
        int ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        while(ll_outgoing_frame_length > 0)
        {
            LLnode * ll_outframe_node = ll_pop_node(&outgoing_frames_head);
            char * char_buf = (char *) ll_outframe_node->value;
            
            //The following function frees the memory for the char_buf object
            send_msg_to_senders(char_buf);

            //Free up the ll_outframe_node
            free(ll_outframe_node);

            ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        }
    }
    pthread_exit(NULL);

}
