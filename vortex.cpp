// The VortexMaker Project, version 1.0
// [Main sources & docs]

// Index of this file:

//  [Includes]
//  [Context Utility: RegisterProvider] => Register a Provider into the context

//-------------------------------------------------------------------------
// [INCLUDES]
//-------------------------------------------------------------------------

#include "vortex.h"
#ifndef VORTEX_DISABLE
#include "vortex_internals.h"

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
//-----------------------------------------------------------------------------
// Current runtime pointer.
#ifndef CVortexMaker
VxContext *CVortexMaker = NULL;
#endif

// Memory Allocator functions. Use SetAllocatorFunctions() to change them.
// - You probably don't want to modify that mid-program, and if you use
// global/static e.g. ImVector<> instances you may need to keep them accessible
// during program destruction.
// - DLL users: read comments above.
#ifndef DISABLE_DEFAULT_ALLOCATORS
static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(user_data);
  return malloc(size);
}
static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(user_data);
  free(ptr);
}
#else
static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(user_data);
  VX_UNUSED(size);
  VX_ASSERT(0);
  return NULL;
}
static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(user_data);
  VX_UNUSED(ptr);
  VX_ASSERT(0);
}
#endif
static VortexMakerMemAllocFunc CVxAllocatorAllocFunc = MallocWrapper;
static VortexMakerMemFreeFunc CVxAllocatorFreeFunc = FreeWrapper;
static void *CVxAllocatorUserData = NULL;

//-----------------------------------------------------------------------------
// [Context Function: CreateContext] => Create a context for a VortexMaker use
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

VORTEX_API VxContext *VortexMaker::CreateContext() {
  /*
  VxContext *context = new VxContext();
  // Vx_NEW
  // malloc(sizeof(context));
  SetCurrentContext(context);
  return context;
  */

  VxContext *prev_ctx = GetCurrentContext();
  VxContext *ctx = VX_NEW(VxContext);
  SetCurrentContext(ctx);
  Initialize();
  if (prev_ctx != NULL)
    SetCurrentContext(
        prev_ctx); // Restore previous context if any, else keep new one.

  return ctx;
}

//-----------------------------------------------------------------------------
// [Context Function: SetCurrentContext] => Set the main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void VortexMaker::SetCurrentContext(VxContext *ctx) {
#ifdef USE_CURRENT_CONTEXT_FUNC
  USE_CURRENT_CONTEXT_FUNC(ctx); // For custom thread-based hackery you may want
                                 // to have control over this.
#else
  CVortexMaker = ctx;
#endif
}

//-----------------------------------------------------------------------------

void VortexMaker::SetAllocatorFunctions(VortexMakerMemAllocFunc alloc_func,
                                   VortexMakerMemFreeFunc free_func,
                                   void *user_data) {
  CVxAllocatorAllocFunc = alloc_func;
  CVxAllocatorFreeFunc = free_func;
  CVxAllocatorUserData = user_data;
}

// This is provided to facilitate copying allocators from one static/DLL
// boundary to another (e.g. retrieve default allocator of your executable
// address space)
void VortexMaker::GetAllocatorFunctions(VortexMakerMemAllocFunc *p_alloc_func,
                                   VortexMakerMemFreeFunc *p_free_func,
                                   void **p_user_data) {
  *p_alloc_func = CVxAllocatorAllocFunc;
  *p_free_func = CVxAllocatorFreeFunc;
  *p_user_data = CVxAllocatorUserData;
}

//-----------------------------------------------------------------------------
// [Context Function: GetCurrentContext] => Get the current main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VORTEX_API void VortexMaker::DestroyContext(VxContext *ctx) {
  VxContext *prev_ctx = GetCurrentContext();
  if (ctx == NULL) //-V1051
    ctx = prev_ctx;
  SetCurrentContext(ctx);
  SetCurrentContext((prev_ctx != ctx) ? prev_ctx : NULL);
  VX_DELETE(ctx);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [Context Function: GetCurrentContext] => Get the current main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VORTEX_API VxContext *VortexMaker::GetCurrentContext() { return CVortexMaker; }
//-----------------------------------------------------------------------------


VORTEX_API void VortexMaker::Initialize() {
  VxContext &ctx = *CVortexMaker;

  // Set initialized flags
  ctx.initialized = true;
}

// IM_ALLOC() == ImGui::MemAlloc()
void *VortexMaker::MemAlloc(size_t size) {
  void *ptr = (*CVxAllocatorAllocFunc)(size, CVxAllocatorUserData);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
  if (VxContext *ctx = CVortexMaker)
    DebugAllocHook(&ctx->debugAllocInfo, ptr, size);
#endif
  return ptr;
}

// Vx_FREE() == VortexMaker::MemFree()
void VortexMaker::MemFree(void *ptr) {
  if (ptr)
    if (VxContext *ctx = CVortexMaker)
      ctx->IO.MetricsActiveAllocations--;
  return (*CVxAllocatorFreeFunc)(ptr, CVxAllocatorUserData);
}

// We record the number of allocation in recent frames, as a way to
// audit/sanitize our guiding principles of "no allocations on idle/repeating
// frames"
void VortexMaker::DebugAllocHook(VortexMakerDebugAllocInfo *info, void *ptr,
                            size_t size) {
  // VortexMakerDebugAllocEntry* entry = &info->LastEntriesBuf[info->LastEntriesIdx];
  VX_UNUSED(ptr);

  /*
  if (entry->FrameCount != frame_count)
  {
      info->LastEntriesIdx = (info->LastEntriesIdx + 1) %
  IM_ARRAYSIZE(info->LastEntriesBuf); entry =
  &info->LastEntriesBuf[info->LastEntriesIdx]; entry->FrameCount = frame_count;
      entry->AllocCount = entry->FreeCount = 0;
  }
  if (size != (size_t)-1)
  {
      entry->AllocCount++;
      info->TotalAllocCount++;
      //printf("[%05d] MemAlloc(%d) -> 0x%p\n", frame_count, size, ptr);
  }
  else
  {
      entry->FreeCount++;
      info->TotalFreeCount++;
      //printf("[%05d] MemFree(0x%p)\n", frame_count, ptr);
  }*/
}


std::string SearchFilesRecursive(const fs::path& chemin, const std::string& filename, std::vector<std::string>& file) {
    for (const auto& entry : fs::directory_iterator(chemin)) {
        if (entry.is_regular_file() && entry.path().filename().string().find(filename) != std::string::npos) {
            std::cout << "Ajout d'un nouveau fichier : " << entry.path().string() << std::endl;
            file.push_back(entry.path().string());
            return entry.path().string();
        } else if (entry.is_directory()) {
            SearchFilesRecursive(entry.path(), filename, file);
        }
    }
    return "null";
}


VORTEX_API std::vector<std::string> VortexMaker::SearchFiles(const std::string& path, const std::string& filename) {
    std::vector<std::string> fichiersTest;
    SearchFilesRecursive(fs::current_path() / path, filename, fichiersTest);
    return fichiersTest;
}

VORTEX_API nlohmann::json VortexMaker::DumpJSON(const std::string& file) {
    std::ifstream fichier(file);
    
    if (!fichier.is_open()) {
        throw std::runtime_error("Error while opening file " + file);
    }

    nlohmann::json json_data;
    fichier >> json_data;

    return json_data;
}

//-----------------------------------------------------------------------------
// [SECTION] ImGuiTextBuffer, ImGuiTextIndex
//-----------------------------------------------------------------------------

// On some platform vsnprintf() takes va_list by reference and modifies it.
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to
// 2013 doesn't have it.
#ifndef va_copy
#if defined(__GNUC__) || defined(__clang__)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#else
#define va_copy(dest, src) (dest = src)
#endif
#endif

char hString::EmptyString[1] = {0};

void hString::append(const char *str, const char *str_end) {
  int len = str_end ? (int)(str_end - str) : (int)strlen(str);

  // Add zero-terminator the first time
  const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
  const int needed_sz = write_off + len;
  if (write_off + len >= Buf.Capacity) {
    int new_capacity = Buf.Capacity * 2;
    Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
  }

  Buf.resize(needed_sz);
  memcpy(&Buf[write_off - 1], str, (size_t)len);
  Buf[write_off - 1 + len] = 0;
}


bool VortexMaker::DebugCheckVersionAndDataLayout(const char* version)
{
    bool error = false;
    if (strcmp(version, VORTEX_VERSION) != 0) { error = true; VX_ASSERT(strcmp(version, VORTEX_VERSION) == 0 && "Mismatched version string!"); }
    return !error;
}

std::string replacePlaceholders(const std::string &command,
                                           const std::unordered_map<std::string, std::string> &replacements)
    {
        std::string result = command;
        for (const auto &[placeholder, value] : replacements)
        {
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos)
            {
                result.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
        return result;
    }



bool RegisterPackage(nlohmann::json packageData) {
  // Global package registering

  return true;
}

bool RegisterToolchain(nlohmann::json toolchainData) {
  VxContext &ctx = *CVortexMaker;
  VxToolchain toolchain;

  toolchain.name      = toolchainData["toolchain"]["name"].get<std::string>();
  toolchain.author    = toolchainData["toolchain"]["author"].get<std::string>();
  toolchain.type      = toolchainData["toolchain"]["type"].get<std::string>();
  toolchain.state     = toolchainData["toolchain"]["state"].get<std::string>();
  toolchain.vendor     = toolchainData["toolchain"]["vendor"].get<std::string>();
  toolchain.platform     = toolchainData["toolchain"]["platform"].get<std::string>();

  toolchain.target_arch     = toolchainData["configs"]["target_arch"].get<std::string>();
  toolchain.host_arch       = toolchainData["configs"]["host_arch"].get<std::string>();
  toolchain.builder_arch    = toolchainData["configs"]["builder_arch"].get<std::string>();


  nlohmann::json packages = toolchainData["content"]["packages"];
  for(auto &pkg : packages){
    toolchain.RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }

  toolchain.FindPackages();


  
  ctx.IO.toolchains.push_back(toolchain);

  return true;
}




VORTEX_API void VortexMaker::InitProject(nlohmann::json main_configs){
  VxContext &ctx = *CVortexMaker;

  ctx.author = main_configs["project"]["author"].get<std::string>();
  ctx.description = main_configs["project"]["description"].get<std::string>();
  ctx.label = main_configs["project"]["label"].get<std::string>();
  ctx.name = main_configs["project"]["name"].get<std::string>();
  ctx.type = main_configs["project"]["type"].get<std::string>();
  ctx.version = main_configs["project"]["version"].get<std::string>();

  ctx.packagesPath = main_configs["data"]["packages"].get<std::string>();
  ctx.toolchainsPath = main_configs["data"]["toolchains"].get<std::string>();

  ctx.paths.toolchainDistFolder = main_configs["dists"]["toolchains"].get<std::string>();

  ctx.projectPath = fs::current_path();

    // Packages
    for (const auto& file : SearchFiles(ctx.packagesPath, "package.config")) {
        try {
            nlohmann::json filecontent = DumpJSON(file);
            RegisterPackage(filecontent);
        } catch (const std::exception& e) {
            std::cerr << "Error : " << e.what() << std::endl;
        }
    }

    // Toolchains
    for (const auto& file : SearchFiles(ctx.toolchainsPath, "toolchain.config")) {
        try {
            nlohmann::json filecontent = DumpJSON(file);
            RegisterToolchain(filecontent);
        } catch (const std::exception& e) {
            std::cerr << "Error : " << e.what() << std::endl;
        }
    }
}

void VxToolchain::FindPackages(){
    VxContext &ctx = *CVortexMaker;

    // Register all finded local packages
    for (const auto& file : VortexMaker::SearchFiles(ctx.toolchainsPath + localPackagesPath, "package.config")) {
        try {
            nlohmann::json filecontent = VortexMaker::DumpJSON(file);
            VxPackage newPackage;

            // Get packages infos

            for(auto registeredPackage : this->registeredPackages){
                if(registeredPackage->emplacement == "local"){
                if(registeredPackage->label == filecontent["package"]["label"].get<std::string>()){
                    std::shared_ptr<VxPackage> newPackage = std::make_shared<VxPackage>();

                    newPackage->path = file;
                    size_t position = newPackage->path.find("/package.config");

                    if (position != std::string::npos) {
                      newPackage->path.erase(position, 17);
                    }

                    newPackage->label = filecontent["package"]["label"].get<std::string>();
                    newPackage->description = filecontent["package"]["description"].get<std::string>();
                    newPackage->name = filecontent["package"]["name"].get<std::string>();
                    newPackage->priority = filecontent["package"]["priority"].get<int>();

                    for(auto configSuffixes : filecontent["compilation"]["configurationSuffixes"]){
                      newPackage->compilation.configurationSuffixes.push_back(configSuffixes);
                    }

                    for(auto configParameters : filecontent["compilation"]["configurationParameters"]){
                      newPackage->compilation.configurationParameters.push_back(configParameters);
                    }

                    this->packages.push_back(newPackage);
                    registeredPackage->resolved = true;
                }
            }
            }

        } catch (const std::exception& e) {
            std::cerr << "Error : " << e.what() << std::endl;
        }
    }

    // Register global packages
    for(auto registeredPackage : this->registeredPackages){
      if(registeredPackage->emplacement == "global"){
          // Recupérer les packages du ctx.
      }

    }
}


    void VxToolchain::PreBuild(){
        VxContext &ctx = *CVortexMaker;
        std::string envPath = ctx.projectPath / ctx.paths.toolchainDistFolder;
            
      std::string baseDir = envPath + "/" + this->name;
      if (mkdir(baseDir.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier baseDir");}

      std::string crosstoolsDir = baseDir + "/" + this->GetTriplet("target");
      if (mkdir(crosstoolsDir.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier crosstoolsDir");}

      this->crosstoolsPath = crosstoolsDir;

      std::string data = baseDir + "/data";
      if (mkdir(data.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier crosstoolsDir");}

      std::string packages_data = baseDir + "/data/packages";
      if (mkdir(packages_data.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier crosstoolsDir");}

      std::string patchs_data = baseDir + "/data/patchs";
      if (mkdir(packages_data.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier crosstoolsDir");}

      std::string scripts_data = baseDir + "/data/scripts";
      if (mkdir(packages_data.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier crosstoolsDir");}

      std::string sysrootDir = crosstoolsDir + "/sysroot";
      if (mkdir(sysrootDir.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier sysrootDir");}

      std::string debugrootDir = crosstoolsDir + "/debugroot";
      if (mkdir(debugrootDir.c_str(), 0777) == -1) {perror("Erreur lors de la création du dossier debugrootDir");}

      for(auto package : this->packages){
        std::string cmd = "cp -r " + package->path + " " + packages_data;
        system(cmd.c_str());
      }

      // other assets

      std::string CMD_AddToGroup = "groupadd vortex";
      std::string CMD_AddUser = "useradd -s /bin/bash -g vortex -m -k /dev/null vortex"; // + " -p " + user.vxHostUser_Crypto;
      std::string CMD_CreateUserDirectory = "mkdir -pv /home/vortex";
      std::string CMD_GiveUserDirectoryToUser = "sudo chown -v vortex:vortex  /home/vortex";

      system((char *)CMD_AddToGroup.c_str());
      system((char *)CMD_AddUser.c_str());


      // Give toolchain to user
      std::string cmd = "sudo chown -v -R vortex " + baseDir+ "/*";
      system((char *)cmd.c_str());
  
    }


    void VxToolchain::Build(){

        std::unordered_map<std::string, std::string> replacements = {
            {"${CrossTools}", this->crosstoolsPath}
        };
            

        for(auto packageToBuild : this->packages){

          // Uncompress 

          // Configure
          std::string configuration;
          if(packageToBuild->compilation.exclusiveCustomConfigProcess == "not specified"){
            std::string cmd = "";

            for(auto suffix : packageToBuild->compilation.configurationSuffixes){
              cmd += suffix + " ";
            }

            cmd += "../configure ";

            for(auto parameter : packageToBuild->compilation.configurationParameters){
              cmd += parameter + " ";
            }

            configuration = replacePlaceholders(cmd, replacements);
          }
          else{
            configuration = replacePlaceholders(packageToBuild->compilation.exclusiveCustomConfigProcess, replacements);
          }
          std::cout << "Configuration : " << configuration << std::endl;
        }


    }

    void VxToolchain::PostBuild(){
      std::string cmd = "userdel -r vortex";
      system((char *)cmd.c_str());
    }

    std::string VxToolchain::GetTriplet(std::string triplet_type)
    {
        std::string NativeTriplet;
        FILE *pipe = popen("gcc -dumpmachine", "r");
        if (!pipe)
        {
            perror("Error while try to get triplet with gcc !");
            return "unknow";
        }
        char buffer[128];
        while (fgets(buffer, 128, pipe) != NULL)
        {
            NativeTriplet += buffer;
        }
        pclose(pipe);
        NativeTriplet.pop_back(); 

        if (triplet_type == "target")
        {

            if (this->type == "native")
            {
              return NativeTriplet;
            }
            else if (this->type == "cross")
            {
              return NativeTriplet;
            }
            else if (this->type == "cross-native" || this->type == "canadian" || this->type == "custom")
            {
                std::string SpecifiedTriplet;

                SpecifiedTriplet += this->target_arch;
                SpecifiedTriplet += "-";
                SpecifiedTriplet += this->vendor;
                SpecifiedTriplet += "-";
                SpecifiedTriplet += this->platform;

                return SpecifiedTriplet;
            }
            else
            {
                return "unknow";
            }
        }
        else if(triplet_type == "builder")
        {

            if (this->type == "native")
            {
                return NativeTriplet;
            }
            else if (this->type == "cross")
            {
                // return crosstoolchain wanted arch
            }
            else if (this->type == "cross-native")
            {
                // return crosstoolchain wanted arch
            }
            else if (this->type == "canadian")
            {
                // return crosstoolchain wanted arch
            }
            else if (this->type == "custom")
            {
                // -> Return Custom triplet
            }
            else
            {
                return "unknow";
            }
        }

        if (triplet_type == "host")
        {

            if (this->type == "native")
            {
                return NativeTriplet;
            }
            else if (this->type == "cross")
            {
                // return crosstoolchain wanted arch
            }
            else if (this->type == "cross-native")
            {
                // return crosstoolchain wanted arch
            }
            else if (this->type == "canadian")
            {
                // return crosstoolchain wanted arch
            }
            else if (this->type == "custom")
            {
                // -> Return Custom triplet
            }
            else
            {
                return "unknow";
            }
        }

        return "unknow";
    }


#endif // VORTEX_DISABLE