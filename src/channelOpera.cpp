/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channelOpera.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lahammam <lahammam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/16 08:21:29 by lahammam          #+#    #+#             */
/*   Updated: 2023/03/16 13:53:24 by lahammam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

void Server::ft_partCmd(Client &client, std::vector<std::string> cmds)
{
    if (cmds.size() == 1)
        ft_print_error("PART", ERR_NEEDMOREPARAMS, client);
    else
    {
        std::vector<std::string> chanls;
        chanls = ft_split(cmds[1], ',');
        for (size_t l = 0; l < chanls.size(); l++)
        {
            int indx = is_channel_Exit(_channels, chanls[l]);
            if (indx == -1)
                ft_print_error(chanls[l], ERR_NOSUCHCHANNEL, client);
            else
            {
                int indexUser = _channels[indx].is_userInChannel(client);
                if (indexUser == -1)
                    ft_print_error(chanls[l], ERR_NOTONCHANNEL, client);
                else
                    _channels[indx].eraseUser(indexUser);
            }
        }
    }
};

int Server::is_userExist(std::string nk)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (strcmp(nk.c_str(), _clients[i].getNickName().c_str()) == 0)
            return i;
    }
    return -1;
};

// :punch.wa.us.dal.net 341 LKOOOLKO LKOOO #OOOOO
// :punch.wa.us.dal.net NOTICE @#OOOOO :LKOOOLKO invited LKOOO into channel #OOOOO

// :LKOOOLKO!~FASHGJFSA@d2a6-9017-cfb7-6374-1329.iam.net.ma INVITE LKOOO :#OOOOO

void Server::ft_inviteCmd(Client &client, std::vector<std::string> cmds)
{
    if (cmds.size() < 3)
        ft_print_error("INVITE", ERR_NEEDMOREPARAMS, client);
    else
    {
        int userIndex = is_userExist(cmds[1]);
        if (userIndex == -1)
            ft_print_error(cmds[1], ERR_NOSUCHNICK, client);
        else
        {
            int indexCha = is_channel_Exit(_channels, cmds[2]);
            if (indexCha == -1)
                ft_print_error(cmds[2], ERR_NOSUCHCHANNEL, client);
            else
            {
                int temp = _channels[indexCha].is_userInChannel(client);
                if (temp == -1)
                    ft_print_error(cmds[2], ERR_NOTONCHANNEL, client);
                else
                {
                    int userInChannel = _channels[indexCha].is_userInChannel(_clients[userIndex]);
                    if (userInChannel != -1)
                        ft_print_error(_channels[indexCha].get_chanlName(), ERR_USERONCHANNEL, client);
                    else
                    {
                        if (_channels[indexCha].ft_isOperator(client) == false)
                            ft_print_error(_channels[indexCha].get_chanlName(), ERR_CHANOPRIVSNEEDED, client);
                        else
                        {
                            client.setMsgTemp(_clients[userIndex].getNickName());
                            ft_print_error(_channels[indexCha].get_chanlName(), RPL_INVITING, client);
                            client.setMsgTemp("");
                            _channels[indexCha].add_userbyInveted(client, _clients[userIndex]);
                            // send msg to geust
                            std::string msg;
                            msg = ":" + client.getNickName() + "!@localhost  INVITE " + _clients[userIndex].getNickName() + " " + _channels[indexCha].get_chanlName() + "\n";
                            send(_clients[userIndex].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
                        }
                    }
                }
            }
        }
    }
};

void Server::ft_kickCmd(Client &client, std::vector<std::string> cmds, char *buffer)
{
    if (cmds.size() < 3)
        ft_print_error("KICK", ERR_NEEDMOREPARAMS, client);
    else
    {
        int userIndex = is_userExist(cmds[1]);
        if (userIndex == -1)
            ft_print_error(cmds[1], ERR_NOSUCHNICK, client);
        else
        {
            int indexCha = is_channel_Exit(_channels, cmds[2]);
            if (indexCha == -1)
                ft_print_error(cmds[2], ERR_NOSUCHCHANNEL, client);
            else
            {
                int temp = _channels[indexCha].is_userInChannel(client);
                if (temp == -1)
                    ft_print_error(cmds[2], ERR_NOTONCHANNEL, client);
                else
                {
                    int userInChannel = _channels[indexCha].is_userInChannel(_clients[userIndex]);
                    if (userInChannel == -1)
                        ft_print_error(_channels[indexCha].get_chanlName(), ERR_USERNOTINCHANNEL, client);
                    else
                    {
                        if (_channels[indexCha].ft_isOperator(client) == false)
                            ft_print_error(_channels[indexCha].get_chanlName(), ERR_CHANOPRIVSNEEDED, client);
                        else
                        {
                            // :USER2222!~KJHGAkj@d2a6-9017-cfb7-6374-1329.iam.net.ma KICK #AHAMMAM USER0000 :USER2222
                            // :USER2222!~KJHGAkj@d2a6-9017-cfb7-6374-1329.iam.net.ma KICK #AHAMMAM USER6666 :BLABLABAL
                            std::string msg;
                            msg = ":" + client.getNickName() + "!@localhost  KICK " + _channels[indexCha].get_chanlName() + " " + _clients[userIndex].getNickName() + " :";
                            if (cmds.size() == 3)
                                msg = msg + client.getNickName() + "\n";
                            else
                            {
                                if (cmds[3][0] != ':')
                                    msg = msg + cmds[3] + "\n";
                                else
                                    msg = msg + strchr(buffer, ':') + "\n";
                            }
                            for (size_t i = 0; i < _channels[indexCha].get_chanlUsers().size(); i++)
                            {
                                send(_channels[indexCha].get_chanlUsers()[i].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
                            }
                            _channels[indexCha].eraseUser(userInChannel);
                        }
                    }
                }
            }
        }
    }
};
