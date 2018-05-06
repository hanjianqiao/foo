#include <SDL2/SDL.h>
#include <pthread.h>
#include <iostream>

using namespace std;

#define MUS_PATH "hello.wav"

// prototype for our audio callback
// see the implementation for more information
void my_audio_callback(void *userdata, Uint8 *stream, int len);

// variable declarations
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play


/*
** PLAYING A SOUND IS MUCH MORE COMPLICATED THAN IT SHOULD BE
*/
void *say(void *data){

    // Initialize SDL.
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
            return (void*)1;

    // local variables
    static Uint32 wav_length; // length of our sample
    static Uint8 *wav_buffer; // buffer containing our audio file
    static SDL_AudioSpec wav_spec; // the specs of our piece of music
    
    
    /* Load the WAV */
    // the specs, length and buffer of our wav are filled
    if( SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
      return (void*)1;
    }
    // set the callback function
    wav_spec.callback = my_audio_callback;
    wav_spec.userdata = NULL;
    // set our global static variables
    audio_pos = wav_buffer; // copy sound buffer
    audio_len = wav_length; // copy file length
    
    /* Open the audio device */
    if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
      fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
      exit(-1);
    }
    
    /* Start playing */
    SDL_PauseAudio(0);

    // wait until we're don't playing
    while ( audio_len > 0 ) {
        SDL_Delay(100); 
    }
    
    // shut everything down
    SDL_CloseAudio();
    SDL_FreeWAV(wav_buffer);
    return (void*)0;
}

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
    
    if (audio_len ==0)
        return;
    
    len = ( len > audio_len ? audio_len : len );
    SDL_memcpy (stream, audio_pos, len);                  // simply copy from one buffer into the other
    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another
    
    audio_pos += len;
    audio_len -= len;
}

int main(){
    pthread_t t;
    int i = 9;
    int rc;
    rc = pthread_create(&t, NULL, say, (void *)i);
    if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
    }
    pthread_join(t, NULL);
    return 0;
}
