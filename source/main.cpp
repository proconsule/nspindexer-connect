#include <switch.h>

#include <stdio.h>
#include <ctype.h>

#include <SDL.h>
#include <glad/glad.h>

#include <vector>
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SimpleIni.h"

#include "windows.h"
#include "popups.h"
#include "TitleManager.h"
#include "curlDownloader.h"
#include "ServerTitles.h"
#include "globals.h"


Tex dummyNSP;
Tex dummyNSZ;
Tex dummyXCI;
Tex dummyXCZ;


using namespace std;
const GLuint WIDTH = 1280, HEIGHT = 720;

SDL_Window *window;
SDL_GLContext context;

static bool init();

enum SDL_KEYS {
		SDL_KEY_A, SDL_KEY_B, SDL_KEY_X, SDL_KEY_Y,
		SDL_KEY_LSTICK, SDL_KEY_RSTICK,
		SDL_KEY_L, SDL_KEY_R,
		SDL_KEY_ZL, SDL_KEY_ZR,
		SDL_KEY_PLUS, SDL_KEY_MINUS,
		SDL_KEY_DLEFT, SDL_KEY_DUP, SDL_KEY_DRIGHT, SDL_KEY_DDOWN,
		SDL_KEY_LSTICK_LEFT, SDL_KEY_LSTICK_UP, SDL_KEY_LSTICK_RIGHT, SDL_KEY_LSTICK_DOWN,
		SDL_KEY_RSTICK_LEFT, SDL_KEY_RSTICK_UP, SDL_KEY_RSTICK_RIGHT, SDL_KEY_RSTICK_DOWN,
		SDL_KEY_SL_LEFT, SDL_KEY_SR_LEFT, SDL_KEY_SL_RIGHT, SDL_KEY_SR_RIGHT
	};


static bool init() {
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0 ){
        printf("%s: SDL could not initialize! SDL Error: %s", __func__, SDL_GetError());
        success =  false;
    }
    else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        window = SDL_CreateWindow(
                "[glad] GL with SDL",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                WIDTH, HEIGHT,
                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        if( window == NULL ){
            printf("%s: Window could not be created! SDL Error: %s", __func__, SDL_GetError());
            success =  false;
        }
        else {
            context = SDL_GL_CreateContext(window);
            if( context == NULL )
            {
                printf( "%s: OpenGL context could not be created! SDL Error: %s", __func__, SDL_GetError());
                success =  false;
            }
            else {
                gladLoadGL();
            }
        }
    }
    return success;
}

bool TxtLoadFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height)
{
	
	FILE * file = fopen(filename.c_str(), "r");
	if (file == NULL) return false;
	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fclose(file);
	// Reading data to array of unsigned chars
	file = fopen(filename.c_str(), "r");
	unsigned char * in = (unsigned char *) malloc(size);
	fread(in, sizeof(unsigned char), size, file);
	
	
	
    int image_width = 0;
    int image_height = 0;
    if (in == NULL){
        return false;
	}
		
	int jpegSubsamp;
	int _jpegSize = 0x20000;
	
	tjhandle _jpegDecompressor = tjInitDecompress();

	tjDecompressHeader2(_jpegDecompressor, in, _jpegSize, &image_width, &image_height, &jpegSubsamp);

	unsigned char buffer[image_width*image_height*3];

	tjDecompress2(_jpegDecompressor, in, _jpegSize, buffer, image_width, 0/*pitch*/, image_height, TJPF_RGB,  TJFLAG_FASTDCT);

	tjDestroy(_jpegDecompressor);

	GLuint id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    

    *out_texture = id;
    *out_width = image_width;
    *out_height = image_height;
	
	fclose(file);
	free(in);

    return true;
}


int main() {
	
	
		CSimpleIniA ini;
		ini.SetUnicode();

		SI_Error rc = ini.LoadFile("config.ini");
		if (rc < 0) {
			rc = ini.LoadFile("/switch/nspindexer-connect/config.ini");
		}	
		if(rc<0){
			consoleInit(NULL);
			padConfigureInput(1, HidNpadStyleSet_NpadStandard);

			PadState pad;
			padInitializeDefault(&pad);

			printf("\x1b[16;20HFailed to load config.ini file, press + to exit");

			while(appletMainLoop())
			{
				padUpdate(&pad);
				u64 kDown = padGetButtonsDown(&pad);

				if (kDown & HidNpadButton_Plus) break;

				consoleUpdate(NULL);
			}

				consoleExit(NULL);
				return 0;
		};
		const char* serverUrl;
		serverUrl = ini.GetValue("Main", "serveraddress", "default");
		
	
    
		Result ret;
		if (R_FAILED(ret = romfsInit())) {
			printf("romfsInit() failed: 0x%x\n", ret);
			return ret;
		}
		ret = ncmInitialize();
		ret = nsInitialize();
	 
		plInitialize(PlServiceType_System);

	 
	    socketInitializeDefault();
#ifdef DEBUG_NXLINK
        nxlinkStdio(); 
#endif
		consoleInit(NULL);
		printf("Getting Titles installed\n");
		
		TitleManager *mytitlemanager = new TitleManager();
		vector<Title> mytitles = mytitlemanager->SearchTitles(NcmContentMetaType_Unknown,NcmStorageId_SdCard);
		printf("Titles found: %ld\n",mytitles.size());
		curlDownloader *mycurl = nullptr;
		curlDownloader *mycurl_serverconfig = nullptr;
	    ServerTitles* myservertitles = nullptr;
		
		mycurl = new curlDownloader();
		bool okdownload = mycurl->download(string(serverUrl)+"?titles");
		if(okdownload){
			printf("Server Titles downloaded successful\n");
		}else
		{
			printf("Error Downloading Data\n");
			sleep(3);
			consoleExit(NULL);
			nsExit();
			ncmExit();
			socketExit();
			return 0;
		}
		
		mycurl_serverconfig = new curlDownloader();
		okdownload = mycurl_serverconfig->download(string(serverUrl)+"?config");
		if(okdownload){
			printf("Server Config downloaded successful\n");
		}else
		{
			printf("Error Downloading Data\n");
			sleep(3);
			consoleExit(NULL);
			nsExit();
			ncmExit();
			socketExit();
			return 0;
		}
		
		
		
		myservertitles = new ServerTitles();
		if(!myservertitles->GetSeverConfig(mycurl_serverconfig->chunk.memory)){
			padConfigureInput(1, HidNpadStyleSet_NpadStandard);

			PadState pad;
			padInitializeDefault(&pad);

			printf("Press + to exit\n");

			while(appletMainLoop())
			{
				padUpdate(&pad);
				u64 kDown = padGetButtonsDown(&pad);

				if (kDown & HidNpadButton_Plus) break;

				consoleUpdate(NULL);
			}
			consoleExit(NULL);
			nsExit();
			ncmExit();
			socketExit();
			return 0;
			
		}
		myservertitles->GetServerTitles(mycurl->chunk.memory);
		myservertitles->Match(mytitles);
		if(okdownload){
			printf("Server info downloaded successful\n");
		}else
		{
			consoleExit(NULL);
			nsExit();
			ncmExit();
			socketExit();
			return 0;
		}

		consoleExit(NULL);
    if ( init() ) {
		
		
		
		
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		io.IniFilename = nullptr;
		io.MouseDrawCursor = false;
        
        ImGui::StyleColorsDark();
		
		ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        Result ret = 0;
		PlFontData standard, fonts_ext;
		if (R_FAILED(ret = plGetSharedFontByType(&standard, PlSharedFontType_Standard))) {
			printf("plGetSharedFontByType(PlSharedFontType_Standard) failed: 0x%x\n", ret);
			return ret;
		}
		
		 plGetSharedFontByType(&fonts_ext, PlSharedFontType_NintendoExt);

		
		unsigned char *pixels = nullptr;
		int width = 0, height = 0, bpp = 0;
		ImFontConfig font_cfg;
		
		font_cfg.FontDataOwnedByAtlas = false;
		
		io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 18.0f,&font_cfg, io.Fonts->GetGlyphRangesDefault());
		font_cfg.MergeMode = true;
		io.Fonts->AddFontFromMemoryTTF(standard.address, standard.size, 22.0f, &font_cfg, io.Fonts->GetGlyphRangesJapanese());
		
		    static const ImWchar ranges[] =
                {
                    0xE000, 0xE06B,
                    0xE070, 0xE07E,
                    0xE080, 0xE099,
                    0xE0A0, 0xE0BA,
                    0xE0C0, 0xE0D6,
                    0xE0E0, 0xE0F5,
                    0xE100, 0xE105,
                    0xE110, 0xE116,
                    0xE121, 0xE12C,
                    0xE130, 0xE13C,
                    0xE140, 0xE14D,
                    0xE150, 0xE153,
                    0,
                };
				
			static const ImWchar tmranges[] =
                {
					0x2019, 0x2019,
                    0x2122, 0x2122,
					0x2713, 0x2713,
					0x2714, 0x2714,
					0x2716, 0x2716,
					0,
                };
	
        io.Fonts->AddFontFromMemoryTTF (fonts_ext.address, fonts_ext.size, 22.0f, &font_cfg, ranges);
		io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 22.0f,&font_cfg, tmranges);
		
		
		io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height, &bpp);
		io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
		io.Fonts->Build();
		
		
		TxtLoadFromFile("romfs:/dummy-nsp.jpg",&dummyNSP.id,&dummyNSP.width,&dummyNSP.height);
		TxtLoadFromFile("romfs:/dummy-nsz.jpg",&dummyNSZ.id,&dummyNSZ.width,&dummyNSZ.height);
		TxtLoadFromFile("romfs:/dummy-xci.jpg",&dummyXCI.id,&dummyXCI.width,&dummyXCI.height);
		TxtLoadFromFile("romfs:/dummy-xcz.jpg",&dummyXCZ.id,&dummyXCZ.width,&dummyXCZ.height);
		
		
		plExit();
		romfsExit();
		
        int exit = 0;
		
		int detailidx = 0;
		int titlefilter = 0;
		bool showMainOptionPopup = false;
		bool focusonMain = true;
        while (!exit) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
				if (event.type == SDL_JOYBUTTONDOWN) {
					Uint8 button = event.jbutton.button;
					if (button == SDL_KEY_PLUS) {
						exit = 1;
					}
					if (button == SDL_KEY_Y) {
						showMainOptionPopup = true;
					}
					if (button == SDL_KEY_B) {
						if(showMainOptionPopup == true){
							showMainOptionPopup = false;
						}
					}
				} 
				if (event.type == SDL_QUIT){
					exit = 1;
				}           


			}

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();
	   
			if(mycurl == nullptr){
				mycurl = new curlDownloader();
				mycurl->download(string(serverUrl)+"?titles");
				myservertitles = new ServerTitles();
				
				if(!myservertitles->GetSeverConfig(mycurl_serverconfig->chunk.memory)){
					exit=1;
				}
				myservertitles->GetServerTitles(mycurl->chunk.memory);
				myservertitles->Match(mytitles);
			}
			focusonMain = true;
			if(showMainOptionPopup){
				focusonMain = false;
				int tmptitlefilter = Popups::MainOptionsPopup(titlefilter);
				if( tmptitlefilter != titlefilter){
					myservertitles->matchedtitles.clear();
					myservertitles->Match(mytitles,tmptitlefilter);
					titlefilter = tmptitlefilter;
				}
			}
			
			myservertitles->matchedtitles = DetailWindows::DetailServerWindow(myservertitles->matchedtitles,detailidx,myservertitles->myserverconfig);
			detailidx = Windows::LocaltoServerWindow(myservertitles->matchedtitles,focusonMain);
			
			
			
            ImGui::Render();
            glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);

            glClearColor(1.0f, 1.0f, 1.0f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            SDL_GL_SwapWindow(window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(context);
    }
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
	nsExit();
	ncmExit();
    socketExit();
    return 0;
}
