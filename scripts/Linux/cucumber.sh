#!/bin/bash

sudo apt update

# Install ruby
sudo apt install -y ruby-full

# Install cucumber/gherkin (ruby version)
sudo gem install cucumber
sudo gem install rspec-expectations