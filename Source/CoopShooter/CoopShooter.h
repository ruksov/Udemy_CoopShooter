// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace surface
{
    constexpr auto gFleshDefault = EPhysicalSurface::SurfaceType1;
    constexpr auto gFleshVulnerable = EPhysicalSurface::SurfaceType2;
}

namespace collision_channel
{
    constexpr auto gWeapon = ECollisionChannel::ECC_GameTraceChannel1;
}
