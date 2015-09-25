#ifndef FFMPEG_FACADE_STREAMPLAYER_H
#define FFMPEG_FACADE_STREAMPLAYER_H

#include <string>
#include <memory>
#include <boost/noncopyable.hpp>

#pragma warning( push )
#pragma warning( disable : 4100 )

#include <boost/thread.hpp>

#pragma warning( pop )

#include <boost/atomic.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "frame.h"

namespace FFmpeg
{
    namespace Facade
    {
		typedef void(__stdcall *StreamStartedCallback)(uint32_t streamNum);
		typedef void(__stdcall *StreamStoppedCallback)(uint32_t streamNum);
		typedef void(__stdcall *StreamFailedCallback)(uint32_t streamNum);

        struct StreamPlayerParams
        {
            StreamPlayerParams()
				: window(nullptr), streamStartedCallback(nullptr),
				streamStoppedCallback(nullptr), streamFailedCallback(nullptr) {}

            HWND window;
            StreamStartedCallback streamStartedCallback;
			StreamStoppedCallback streamStoppedCallback;
			StreamFailedCallback streamFailedCallback;
        };

        /// <summary>
        /// A StreamPlayer class implements a stream playback functionality.
        /// </summary>
        class StreamPlayer : private boost::noncopyable
        {
        public:

            /// <summary>
            /// Initializes a new instance of the StreamPlayer class.
            /// </summary>
            StreamPlayer();

            /// <summary>
            /// Initializes the player.
            /// </summary>
			/// <param name="playerParams">The StreamPlayerParams object that contains the information that is used to initialize the player.</param>
			void Initialize(StreamPlayerParams playerParams);

            /// <summary>
            /// Asynchronously plays a stream.
            /// </summary>
            /// <param name="streamUrl">The url of a stream to play.</param>
			void StartPlay(std::string const& streamUrl);

			/// <summary>
			/// Asynchronously plays a second (PiP) stream.
			/// </summary>
			/// <param name="streamUrl">The url of a stream to play.</param>
			void StartPlayPiP(std::string const& streamUrl);
			
			/// <summary>
            /// Stops a stream.
            /// </summary>
            void Stop();

            /// <summary>
            /// Retrieves the current frame being displayed by the player.
            /// </summary>
            /// <param name="bmpPtr">Address of a pointer to a byte that will receive the DIB.</param>
            void GetCurrentFrame(uint8_t **bmpPtr);

            /// <summary>
            /// Retrieves the unstretched frame size, in pixels.
            /// </summary>
            /// <param name="widthPtr">A pointer to an int that will receive the width.</param>
            /// <param name="heightPtr">A pointer to an int that will receive the height.</param>
            void GetFrameSize(uint32_t *widthPtr, uint32_t *heightPtr);

			/// <summary>
			/// Set PiP parameters.
			/// </summary>
			void SetupPiP(int32_t *pip_width, int32_t *pip_top, int32_t *pip_left);

			/// <summary>
			/// Set Zoom parameter.
			/// </summary>
			void SetupZoom(int32_t *zoom);

			/// <summary>
			/// Set Cross parameter.
			/// </summary>
			void SetupCross(int32_t *cross);

			/// <summary>
            /// Uninitializes the player.
            /// </summary>
            void Uninitialize();

        private:
			/// <summary>
			/// Plays a stream.
			/// </summary>
			/// <param name="streamUrl">The url of a stream to play.</param>
			void Play(std::string const& streamUrl);

			/// <summary>
			/// Plays a second (PiP) stream.
			/// </summary>
			/// <param name="streamUrl">The url of a stream to play.</param>
			void PlayPiP(std::string const& streamUrl);

			/// <summary>
			/// Draws a frame.
			/// </summary>
            void DrawFrame();

			/// <summary>
			/// Raises the StreamStarted event.
			/// </summary>
            void RaiseStreamStartedEvent(uint32_t streamNum);

			/// <summary>
			/// Raises the StreamStopped event.
			/// </summary>
			void RaiseStreamStoppedEvent(uint32_t streamNum);

			/// <summary>
			/// Raises the StreamFailed event.
			/// </summary>
			void RaiseStreamFailedEvent(uint32_t streamNum);

            static LRESULT APIENTRY WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        private:
			            
            boost::atomic<bool> stopRequested_;
			boost::atomic<bool> stopRequestedPiP_;
			StreamPlayerParams playerParams_;

            std::unique_ptr<Frame> framePtr_;
			std::unique_ptr<Frame> framePiPPtr_;

            // There is a bug in the Visual Studio std::thread implementation,
            // which prohibits dll unloading, that is why the boost::thread is used instead.
            // https://connect.microsoft.com/VisualStudio/feedback/details/781665/stl-using-std-threading-objects-adds-extra-load-count-for-hosted-dll#tabs 

			boost::mutex mutex_;
			boost::mutex mutexPiP_;
			boost::thread workerThread_;
			boost::thread workerThreadPiP_;

            static WNDPROC originalWndProc_;
		
			int pip_width_;
			int pip_top_, pip_left_;
			bool we_have_pip_;
			int zoom_;
			int cross_;
		};
    }
}

#endif // FFMPEG_FACADE_STREAMPLAYER_H