#!/usr/bin/env bash
@echo off
cls
mkdir ndll/Mac64
pushd project
haxelib run hxcpp Build.xml -Dmac
popd