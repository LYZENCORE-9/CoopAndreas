
#include "ConfigDatabase.hpp"


void ConfigDatabase::Init(const std::string &filename)
{
	if(ConfigDatabase::Create(filename))
   	{
	    if(ConfigDatabase::Load(filename))
   	    {
   	        std::cout << "\n[*] : " << filename << " Has Been Created (Please Shutdown Server And Edit " << filename << "!" << '\n'; 
   	    }else { ConfigDatabase::Create(filename); }
   	}else 
   	{
   	    if(ConfigDatabase::Load(filename))
   	    {
    	       std::cout << "\n[*] : " << filename << " Has Been Loaded !" << '\n'; 
   	    }
   	}
}

bool ConfigDatabase::Create(const std::string &filename)
{
	if(!std::filesystem::exists(filename)) 
	{
		FILE *pFile = fopen(filename.c_str(), "w");

		INI::File ft(filename);
		ft.GetSection("Server Settings")->SetValue("Port", 0);

		ft.Save(filename);
		return true;
	}
	return false;
}

bool ConfigDatabase::Load(const std::string &filename)
{
	if(std::filesystem::exists(filename))
	{
		INI::File ft(filename);
		ft.Load(filename);
		ConfigDatabase::configport = ft.GetSection("Server Settings")->GetValue("Port", 1).AsInt();
		return true;
	}
	else 
		printf("ERROR");
	return false;
}