/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
 
#include "FacebookAgent.h"
#include "AgentManager.h"
#include "PluginJniHelper.h"

namespace cocos2d{namespace plugin{

extern "C" {
JNIEXPORT void JNICALL Java_org_cocos2dx_plugin_UserFacebook_nativeRequestCallback(JNIEnv*  env, jobject thiz, jint ret, jstring msg, jint cbIndex)
{
	std::string stdMsg = PluginJniHelper::jstring2string(msg);
	FacebookAgent::FBCallback callback = FacebookAgent::getInstance()->getRequestCallback(cbIndex);
	callback(ret, stdMsg);
}
}

static FacebookAgent* s_sharedFacebookAgent = nullptr;

FacebookAgent* FacebookAgent::getInstance()
{
	if(nullptr == s_sharedFacebookAgent)
	{
		s_sharedFacebookAgent = new FacebookAgent();
	}
	return s_sharedFacebookAgent;
}

void FacebookAgent::destroyInstance()
{
	if(s_sharedFacebookAgent)
	{
		delete s_sharedFacebookAgent;
		s_sharedFacebookAgent = nullptr;
	}
}

FacebookAgent::FacebookAgent()
{
	agentManager = AgentManager::getInstance();
	std::map<std::string, std::string> facebook = {{"PluginUser", "UserFacebook"}, {"PluginShare", "ShareFacebook"}};
	agentManager->init(facebook);
}

FacebookAgent::~FacebookAgent()
{
    requestCallbacks.clear();
	AgentManager::destroyInstance();
}

void FacebookAgent::login(FBCallback cb)
{
	agentManager->getUserPlugin()->login(cb);
}

void FacebookAgent::login(std::string& permissions, FBCallback cb)
{
	auto userPlugin = agentManager->getUserPlugin();
	userPlugin->setCallback(cb);
	PluginParam _permissions(permissions.c_str());
	userPlugin->callFuncWithParam("login", &_permissions, NULL);
}

void FacebookAgent::logout()
{
	agentManager->getUserPlugin()->logout();
}

bool FacebookAgent::isLoggedIn()
{
	return agentManager->getUserPlugin()->isLoggedIn();
}

std::string FacebookAgent::getPermissionList()
{
    return agentManager->getUserPlugin()->callStringFuncWithParam("getPermissionList", NULL);
}

std::string FacebookAgent::getAccessToken()
{
	return agentManager->getUserPlugin()->callStringFuncWithParam("getAccessToken", NULL);
}

void FacebookAgent::share(FBInfo& info, FBCallback cb)
{
	agentManager->getSharePlugin()->share(info, cb);
}

void FacebookAgent::dialog(FBInfo& info, FBCallback cb)
{
	auto sharePlugin = agentManager->getSharePlugin();
	sharePlugin->setCallback(cb);
	PluginParam params(info);
	sharePlugin->callFuncWithParam("dialog", &params, NULL);
}

void FacebookAgent::api(std::string &path, int method, FBInfo &params, FBCallback cb)
{
	requestCallbacks.push_back(cb);

	PluginParam _path(path.c_str());
	PluginParam _method(method);
	PluginParam _params(params);
	PluginParam _cbIndex((int)(requestCallbacks.size() - 1));

	agentManager->getUserPlugin()->callFuncWithParam("request", &_path, &_method, &_params, &_cbIndex, NULL);
}

FacebookAgent::FBCallback FacebookAgent::getRequestCallback(int index)
{
	return requestCallbacks[index];
}

void FacebookAgent::publishInstall()
{
	agentManager->getUserPlugin()->callFuncWithParam("publishInstall", NULL);
}

void FacebookAgent::logEvent(std::string& eventName)
{
	PluginParam _eventName(eventName.c_str());
	agentManager->getUserPlugin()->callFuncWithParam("logEvent", &_eventName, NULL);
}

void FacebookAgent::logEvent(std::string& eventName, float valueToSum)
{
	PluginParam _eventName(eventName.c_str());
	PluginParam _valueToSum(valueToSum);
	agentManager->getUserPlugin()->callFuncWithParam("logEvent", &_eventName, &_valueToSum, NULL);
}

void FacebookAgent::logEvent(std::string& eventName, FBInfo& parameters)
{
	PluginParam _eventName(eventName.c_str());
	PluginParam _params(parameters);
	agentManager->getUserPlugin()->callFuncWithParam("logEvent", &_eventName, &_params, NULL);
}

void FacebookAgent::logEvent(std::string& eventName, float valueToSum, FBInfo& parameters)
{
	PluginParam _eventName(eventName.c_str());
	PluginParam _valueToSum(valueToSum);
	PluginParam _params(parameters);
	agentManager->getUserPlugin()->callFuncWithParam("logEvent", &_eventName, &_valueToSum, &_params, NULL);
}

void FacebookAgent::appRequest(FBInfo& info, FBCallback cb)
{
	auto sharePlugin = agentManager->getSharePlugin();
	sharePlugin->setCallback(cb);
	PluginParam params(info);
	sharePlugin->callFuncWithParam("appRequest", &params, NULL);
}

}}
