//
// Created by ianpo on 01/11/2023.
//

#pragma once

#include <filesystem>
#include <string>
#include <glm/glm.hpp>

namespace Voxymore::Audio
{
	using Vector3 = glm::vec3;
    typedef uint32_t TypeId;

    //TODO: Find a way to save and load all the sound definitions from disk

    /**
     * @struct SoundDefinition
     * @brief Defines the properties of a sound.
     */
     struct SoundDefinition
    {
        std::string name;
        float defaultVolumedB = 30.0f;
        float minDistance = 0.0f;
        float maxDistance = 100.0f;
        bool is3D = true;
        bool isLooping = false;
        bool isStream = false;
    };

    struct OneShotSound
    {
        TypeId SoundId;
        TypeId ChannelId;
    };

	class Voxaudio
	{
	public:
        /**
         * @brief Initialize the application with optional configuration file.
         *
         * This function should be called to initialize the application before performing any other operations.
         * It takes an optional configuration file path that specifies the location of the configuration file
         * to be used for initialization. If no configuration file is provided, the application will be
         * initialized with default settings.
         *
         * @param configFile The path to the configuration file (optional)
         *
         * @Note If the provided configuration file is not found, the default settings will be used.
         *
         * @Note If no configuration file is provided, the application will be initialized with default settings.
         *
         * @Note This function must be called only once.
         */
        static void Init(const std::filesystem::path& configFile = "");

        /**
         * @brief This method updates the game logic based on the elapsed time since the last update.
         *
         * @param deltaTimeSeconds The time elapsed in seconds since the last update.
         *
         * @details This method is invoked to update the game logic and perform necessary calculations based on the elapsed time
         *          since the last update. It can be called in a loop to continuously update the game state.
         *          The deltaTimeSeconds parameter allows for time-based calculations, ensuring that the game logic is updated
         *          at a consistent rate regardless of the machine's performance.
         *          Sample usage:
         *
         *          float deltaTimeSeconds = calculateDeltaTime(); // Calculate the elapsed time since the last update
         *          Update(deltaTimeSeconds); // Invoke the method to update game logic based on the elapsed time
         *
         * @warning This method assumes that certain variables and objects required for game logic are already initialized.
         *          The method should be customized and extended as needed.
         */
        static void Update(float deltaTimeSeconds);

        /**
         * @brief Shutdowns the software application gracefully.
         *
         * This function is used to shutdown the software application gracefully.
         * It performs necessary cleanup tasks and saves any unsaved data before
         * terminating the program.
         *
         * @return void
         */
        static void Shutdown();

        /**
         * @brief Registers a sound with its definition.
         *
         * This function registers a sound using the provided sound definition.
         * Optionally, it can also load the sound data into memory.
         *
         * @param soundDef The sound definition that encapsulates the sound's properties.
         * @param load     Specifies whether to load the sound data into memory.
         *                 Default is true.
         */
        static TypeId RegisterSound(const SoundDefinition& soundDef, bool load = true);

        /**
         * @brief Unregisters a sound with the given sound ID.
         *
         * This function unregisters a sound with the specified sound ID from the system.
         * Once a sound is unregistered, it will no longer be available for playback.
         *
         * @param soundId The unique identifier of the sound to unregister.
         *
         * @return None.
         */
        static void UnregisterSound(TypeId soundId);

        /**
         * @brief Loads the sound with the specified soundId.
         *
         * This function is responsible for loading a sound file based on the given soundId. The soundId is used to identify
         * a unique sound file that needs to be loaded. The sound file should be a valid audio file supported by the system.
         *
         * @param soundId The identifier of the sound to be loaded.
         *
         * @return True if the sound is successfully loaded, false otherwise.
         */
        static void LoadSound(TypeId soundId);

        /**
         * @brief Unloads a sound from memory.
         *
         * This function unloads a sound from memory using the provided sound ID. Once unloaded, the sound can no longer be used
         * until it is reloaded. It is important to unload sounds that are no longer needed in order to free up memory resources.
         *
         * @param soundId The unique identifier of the sound to be unloaded.
         */
        static void UnloadSound(TypeId soundId);

        //TODO: bool ShouldBeVirtual(bool allowOneShotVirtuals) const

        /**
         * @brief Set the 3D listener position and orientation.
         *
         * This function sets the position and orientation of the 3D listener in the virtual 3D space.
         * The position defines the listener's location in the space, while the look and up vectors define
         * the listener's orientation. The look vector specifies the direction the listener is facing,
         * and the up vector specifies the upward direction.
         *
         * @param position The position of the listener in 3D space.
         * @param look The direction the listener is facing.
         * @param up The upward direction for the listener.
         *
         * @note The position, look, and up vectors should be normalized. Non-normalized vectors can result
         *       in unexpected listener behavior.
         *
         * @see Vector3
         */
        static void Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up);

        /**
         * @brief Sets the 3D listener position, orientation, and velocity.
         *
         * @param position The listener position.
         * @param look The direction the listener is facing.
         * @param up The upward direction of the listener.
         * @param velocity The velocity of the listener.
         */
        static void Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up, const Vector3& velocity);


        /**
         * @brief Plays a sound at a specified position with a specified volume.
         *
         * @param soundId The id of the sound to be played.
         * @param pos The position where the sound should be played. Default is {0, 0, 0}.
         * @param volumedB The volume of the sound in decibels. Default is 0.0f.
         *
         * @details This function plays a sound identified by the given soundId at the specified position with the specified volume.
         * If no position is provided, the sound will be played from the default position {0, 0, 0}.
         * If no volume is provided, the default volume of 0.0 decibels will be used.
         * This function does not return any value.
         *
         * Example Usage:
         *     PlaySound(3, {10, 5, 0}, -10.5f); // Plays sound with id 3 at position (10, 5, 0) with volume -10.5 decibels.
         *     PlaySound(5); // Plays sound with id 5 at the default position with the default volume.
         */
        static TypeId PlaySound(TypeId soundId, const Vector3& pos = {0, 0, 0 }, float volumedB = 0.0f);

        /**
         * @brief Plays a sound effect at a specified position with a specified volume.
         *
         * This function plays a given sound effect at a specific position in the world. The sound effect
         * can be defined by a SoundDefinition object. The desired position can be specified using a 3D
         * Vector3 object. In case no position is provided, the sound will be played at the default position
         * {0,0,0}. The user also has the option to specify a volume for the sound effect, which is
         * represented in decibels (dB). If no volume is provided, the default volume of 0.0 dB will be used.
         *
         * @param soundDef The definition of the sound effect to be played.
         * @param pos The position in the world where the sound effect should be played.
         * @param volumedB The desired volume for the sound effect in decibels.
         *
         * @note When using the default position and/or volume, it is recommended to explicitly specify
         *       the default values for better code readability.
         */
        static OneShotSound PlayOnShot(const SoundDefinition& soundDef, const Vector3& pos = {0, 0, 0 }, float volumedB = 0.0f);

        /**
         * StopChannel function stops the audio playback for a specific channel.
         *
         * @param channelId The ID of the channel to be stopped.
         * @param fadeTimeSeconds The fade-out duration in seconds. Default value is 0.0f.
         *
         * @note If the fadeTimeSeconds parameter is set to a positive value, the audio will fade out over the specified duration.
         * @note If the fadeTimeSeconds parameter is set to 0.0f, the audio will stop abruptly without any fade-out effect.
         */
        static void StopChannel(TypeId channelId, float fadeTimeSeconds = 0.0f);

        /**
         * @brief Stops all active channels.
         *
         * This function stops all active channels. It can be used to stop any audio or video playback
         * that is currently in progress. Any ongoing recording or streaming will also be stopped.
         * After calling this function, the channels will be in a stopped state and can be restarted
         * if necessary.
         *
         * @note This function should be called only when it is necessary to stop all channels.
         *       If you want to stop a specific channel, use the StopChannel() function instead.
         *       Calling this function will stop all channels, including ones that you may not want to stop.
         *
         * @return None
         *
         * @see StopChannel()
         */
        static void StopAllChannels();

        /**
         * @brief Set the 3D position of a channel
         *
         * This function sets the 3D position of a specific channel identified by its TypeId.
         * The position is specified using a Vector3 object representing the x, y, and z coordinates.
         *
         * @param channelId The TypeId of the channel whose position is to be set
         * @param position The 3D position to set for the channel
         *
         * @return void
         */
        static void SetChannel3dPosition(TypeId channelId, const Vector3& position);

        /**
         * \brief Set the volume level of a specific channel.
         *
         * \param channelId  The ID of the channel.
         * \param volumedB   The desired volume level in decibels.
         */
        static void SetChannelVolume(TypeId channelId, float volumedB);

        /**
         * @brief Checks if a channel with the given TypeId is currently playing.
         *
         * This function is used to determine if a channel with the specified TypeId
         * is currently in a playing state. It returns true if the channel is playing
         * and false otherwise.
         *
         * @param channelId The TypeId of the channel to be checked.
         * @return Boolean value indicating whether the channel is playing or not.
         */
        static bool IsPlaying(TypeId channelId);
	};

    namespace Helper
    {
        /**
         * @brief Converts a decibel (dB) value to a volume value.
         *
         * The dBToVolume function takes a decibel value and converts it to a volume value. Decibels are a logarithmic unit
         * used to measure the relative intensity of a sound or signal, while volume is a linear value indicating the loudness
         * of a sound.
         *
         * @param dB The input decibel value to be converted to volume.
         * @return The corresponding volume value.
         *
         * @note The dBToVolume function assumes that the input decibel value is within a valid range.
         * @note The dBToVolume function does not validate the input.
         *
         * @see volumeToDB
         */
        float dBToVolume(float dB);

        /**
         * @brief Convert volume to decibels (dB).
         *
         * This function converts a given volume to decibels (dB).
         *
         * @param Volume The volume value to be converted to decibels.
         * @return The corresponding decibel value.
         */
        float VolumeTodB(float Volume);
    }
}